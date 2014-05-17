#include "inode.h"
#include "coreio.h"
#include "control.h"

#ifdef FS_DEBUG_ON
#include <stdio.h>
#endif

/*
  obtains an inode structure from the id of the inode
*/
inode inode_get_inode(uint32 inode_id)
{
  inode tmp = {0};

#ifdef FS_DEBUG_ON
  printf("In function inode_get_inode: \n");
#endif

  if(open_fs == NULL)
    {
#ifdef FS_DEBUG_ON
      printf("Filesystem not initialized. Exit.\n");
#endif
      tmp.mode = IFINVALID;
      return tmp;
    }
  coreio_fseek(open_fs, (sb.inode_table * BLOCK_SIZE) + (inode_id * sizeof(inode)), SEEK_SET);

  coreio_fread(&tmp, sizeof(inode), 1, open_fs);

  return tmp;
}

/*
  checks if an inode is set as valid in the inode bitmap.
  return 0 for false, 1 for true.
*/
uint8 inode_is_valid(uint32 inode_id)
{
  uint8 data = 0;
  uint8 mask = 0x01;

#ifdef FS_DEBUG_ON
  printf("In function inode_is_valid: \n");
#endif

  if(open_fs == NULL)
    return -1;

  coreio_fseek(open_fs, (sb.inode_bitmap * BLOCK_SIZE) + (inode_id / 8), SEEK_SET);

  coreio_fread(&data, sizeof(uint8), 1, open_fs);

  mask = mask << inode_id % 8;

  data = data & mask;

  if(data)
    return 1;
  else
    return 0;
}


/*
Enables an inode for usage in the inode table
 */
int8 inode_set_valid(uint32 inode_id)
{
  inode tmp = {0};

#ifdef FS_DEBUG_ON
  printf("In function inode_set_valid: \n");
#endif

  if(inode_is_valid(inode_id))
    return 0;

  tmp.inode_id = inode_id;

  inode_overwrite_inode(tmp);

  if(!control_modify_inode_bitmap(inode_id, BIT_SET))
    {
#ifdef FS_DEBUG_ON
      printf("Error calling control_modify_inode_bitmap() from inode_set_valid.\n");
#endif
      return -1;
    }

  return 0;
}

/*
  sets an inode as free in the inode table. does not deallocate blocks.
  returns 0 on success
*/
uint8 inode_set_invalid(uint32 inode_id)
{
#ifdef FS_DEBUG_ON
  printf("In function inode_set_invalid: \n");
#endif

  if(!inode_is_valid(inode_id))
    return 0;

  if(!control_modify_inode_bitmap(inode_id, BIT_CLEAR))
    {
#ifdef FS_DEBUG_ON
      printf("Error calling control_modify_inode_bitmap() from inode_set_invalid().\n");
#endif
      return -1;
    }

  return 0;
}

/*
Overwrites an inode in disk with the inode passed to the function.
The inode id is taken from the inode struct itself.
 */
int8 inode_overwrite_inode(inode source_inode)
{
#ifdef FS_DEBUG_ON
  printf("In function inode_overwrite_inode: \n");
#endif

  if(source_inode.mode == IFINVALID)
    {
#ifdef FS_DEBUG_ON
      printf("Invalid inode. Exit.");
#endif
      return -1;
    }

  coreio_fseek(open_fs, (sb.inode_table * BLOCK_SIZE) + (sizeof(inode) * source_inode.inode_id), SEEK_SET);
  
  coreio_fwrite(&source_inode, sizeof(inode), 1, open_fs);
  
  return 0;
}

uint32 inode_find_free_inode()
{
  uint8 mask = 0x01;
  uint8 cur_byte = 0;
  uint32 inode_cntr = sb.inode_count / 8;
  uint32 i = 0;
  uint32 j = 0;
  uint32 k = 0;
  uint8 data[BLOCK_SIZE] = {0};

#ifdef FS_DEBUG_ON
  printf("In function inode_find_free_inode: \n");
#endif

  inode_cntr = inode_cntr / BLOCK_SIZE;

  if(((sb.inode_count / 8) % BLOCK_SIZE) != 0)
    inode_cntr++;

  for(i = 0; i < inode_cntr; i++)
    {
      coreio_read_block(data, 1, sb.inode_bitmap + i);

      for(j = 0; j < BLOCK_SIZE; j++)
	{
	  mask = 0x01;
	  cur_byte = data[j];

	  for(k = 0; k < 8; k++)
	    {
	      if(cur_byte & mask)
		{
		  return (i * BLOCK_SIZE) + (j * k);
		}

	      mask = mask << 1;
	    }
	}
    }

  return 0;
}
