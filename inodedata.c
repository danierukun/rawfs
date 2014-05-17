#include "inodedata.h"
#include "coreio.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


// -------------- Static function forward declarations ---------------
static offset_container inodedata_calculate_offsets(uint32 block_id);
// -------------------------------------------------------------------


/*
  reads a single block of data from an inode reference
*/
uint8* inodedata_read_block(inode cur_inode, uint32 block_id)
{
  offset_container offset = {0};
  uint8* data = NULL;
  uint32 blk_disk_addr = 0;

#ifdef FS_DEBUG_ON
  printf("In function inode_read_block: \n");
#endif

  if(!inode_is_valid(cur_inode.inode_id))
    {
#ifdef FS_DEBUG_ON
      printf("Invalid inode. Exit.");
#endif
      return NULL;
    }

  offset = inodedata_calculate_offsets(block_id);

#ifdef FS_DEBUG_ON

  printf("Inode indirection level: %d\n", offset.indirection_lvl);
 
  int dbg_i = 0;
 
  for(i = 0; dbg_i < offset.indirection_lvl; dbg_i++)
    {
      printf("Indirection index %d: %d\n", i, offset.indirection_index[i]);
    }
#endif

  if(offset.indirection_lvl == -1)
    {
#ifdef FS_DEBUG_ON
      printf("Invalid indirection level. Abort.");
#endif
      return NULL;
    }

  blk_disk_addr = inode_traverse_indirect_blocks(cur_inode, offset);
 
#ifdef FS_DEBUG_ON
  printf("Block address to read: %d\n", blk_disk_addr);
#endif

  coreio_read_block(data, 1, blk_disk_addr);

  return data;
}

/*
  writes a data array and allocates the resulting blocks to an inode
*/
int8 inodedata_write_block(void* data, uint32 data_size, inode cur_inode)
{
  uint32 block_addr = 0;
  offset_container offset = {0};

#ifdef FS_DEBUG_ON
  printf("In function inode_write_block: \n");
#endif

  if(!inode_is_valid(cur_inode.inode_id))
    {
#ifdef FS_DEBUG_ON
      printf("Invalid inode. Return.");
#endif
      return -3;
    }

  if(cur_inode.block_span >= (pow(BLOCK_SIZE / 4, 3) + pow(BLOCK_SIZE / 4, 2) + (BLOCK_SIZE / 4) + 12))
    {
#ifdef FS_DEBUG_ON
      printf("Max inode data block quantity exceeded. Exit.");
#endif

      return -1;
    }

  while(data_size > 0)
    {
      block_addr = control_find_free_block();

      if(block_addr == -1)
	{
#ifdef FS_DEBUG_ON
	  printf("No free blocks found. Abort.\n");
#endif
	  return -1;
	}

#ifdef FS_DEBUG_ON
      printf("Free block found at address %d\n", block_addr);
#endif
      
      offset = inodedata_calculate_offsets(cur_inode.block_span);

      if(!coreio_write_block(data, BLOCK_SIZE, block_addr))
	{
#ifdef FS_DEBUG_ON
	  printf("Error calling coreio_write_block. Abort.\n");
#endif
	  return -2;
	}

      cur_inode.block_span++;

#ifdef FS_DEBUG_ON
      printf("Attempting to modify block bitmap\n");
      printf(" write status: %d\n", control_modify_blk_bitmap(block_addr, BIT_SET));
#else
      control_modify_blk_bitmap(block_addr, BIT_SET);
#endif

      data_size -= BLOCK_SIZE;
    }

  return 0;
}

/*
  deallocates one block from an inode and reflects changes in block table
*/
int8 inodedata_deallocate_block(inode cur_inode, uint32 block_id)
{
  uint8* data = NULL;
  uint32 blk_disk_addr = 0;
  offset_container offset = {0};

#ifdef FS_DEBUG_ON
  printf("In function inode_deallocate_block: \n");
#endif

  if(cur_inode.mode == IFINVALID)
    {
#ifdef FS_DEBUG_ON
      printf("Null pointer received. Return.\n");
#endif

      return -1;
    }

  offset = inodedata_calculate_offsets(block_id);

  blk_disk_addr = inode_traverse_indirect_blocks(cur_inode, offset);

  control_modify_blk_bitmap(blk_disk_addr, BIT_CLEAR);
}

/*
traverses the indirect blocks of an array. returns the disk block address
of the block referenced by the offset container.
 */
uint32 inodedata_traverse_indirect_blocks(inode cur_inode, offset_container offset)
{
  uint8* data = NULL;
  uint32 cur_blk_addr = 0;
  uint32 i = 0;

#ifdef FS_DEBUG_ON
  printf("In function inode_traverse_indirect_blocks: \n");
#endif

#ifdef FS_DEBUG_ON
  printf("Attempting to allocate data buffer of %d bytes.\n", BLOCK_SIZE);
#endif

  data = malloc(BLOCK_SIZE);

#ifdef FS_DEBUG_ON
  printf("Attempting to read inode data block %d (id %d)\n", offset.indirection_index[0], cur_inode.data_blocks[offset.indirection_index[0]]);
  printf("Read status: %d\n", coreio_read_block(data, 1, cur_inode.data_blocks[offset.indirection_index[0]]));
#else
  coreio_read_block(data, 1, cur_inode.data_blocks[offset.indirection_index[0]]);
#endif

  for(i = 1; i < offset.indirection_lvl; i++)
    {
      cur_blk_addr = util_char_array_to_int(data + offset.indirection_index[i]);
      
#ifdef FS_DEBUG_ON
      printf("Attempting to read disk block %d\n", block_addr);
      printf(" Read status: %d\n", coreio_read_block(data, 1, cur_blk_addr));
#else
      coreio_read_block(data, 1, cur_blk_addr);
#endif
    }

#ifdef FS_DEBUG_ON
  printf("Final traversed block address: %d\n", cur_blk_addr);
#endif

  free(data);

  return cur_blk_addr;
}

// ------------------------- static functions ---------------------------

/*
  calculates the indirection offsets of a single block inside an inode
*/
static offset_container inodedata_calculate_offsets(uint32 block_id)
{
  offset_container c = {0};
  uint32 pointer_size = BLOCK_SIZE / 4;

#ifdef FS_DEBUG_ON
  printf("In function inode_calculate_offsets: \n");
#endif

  if(block_id < 12)
    {
      c.indirection_lvl = 0;
      c.indirection_index[0] = block_id;

      return c;
    }

  if(((block_id - 12) / pointer_size) <= 1)
    {
      c.indirection_lvl = 1;
      c.indirection_index[0] = 12;
      c.indirection_index[1] = (block_id - 12) % pointer_size;

      return c;
    }

  if(((block_id - 12) / pointer_size) >= 1)
    {
      c.indirection_lvl = 2;
      c.indirection_index[0] = 13;
      c.indirection_index[1] = (block_id - 12) / pointer_size;
      c.indirection_index[2] = (block_id - 12) % pointer_size;

      return c;
    }

  if(((block_id - 12) / (pointer_size * pointer_size)) >= 1)
    {
      c.indirection_lvl = 3;
      c.indirection_index[0] = 14;
      c.indirection_index[1] = (block_id - 12) / pointer_size;
      c.indirection_index[2] = c.indirection_index[1] / pointer_size;
      c.indirection_index[3] = (block_id - 12) % pointer_size;

      return c;
    }

  c.indirection_lvl = -1;

  return c;
}
