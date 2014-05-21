#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "control.h"
#include "inodedata.h"

inode current_dir = {0};

static int8 control_add_directory_entry(inode target_inode, dir_entry entry);

superblock* control_construct_superblock(uint32 block)
{
  superblock* s = NULL;
  int32 byte_qty = 0;

#ifdef FS_DEBUG_ON
  printf("In function control_construct_superblock: \n");
#endif

  if(open_fs == NULL)
    return NULL;

  s = malloc(sizeof(superblock));

#ifdef FS_DEBUG_ON
  printf("Memory allocated.\n");
#endif

  coreio_fseek(open_fs,0,SEEK_END);
  byte_qty = coreio_ftell(open_fs);
  coreio_rewind(open_fs);

  s->block_size = BLOCK_SIZE;
  s->block_count = byte_qty / BLOCK_SIZE;

  if((byte_qty % BLOCK_SIZE) != 0)
    s->block_count++;

  s->free_blocks = s->block_count - 2;
  s->inode_count = s->block_count / BLOCK_INODE_RATIO;
  s->free_inodes = s->inode_count;

#ifdef FS_DEBUG_ON
  printf("returning superblock\n");
#endif

  return s;
}

int8 coreio_init()
{

}

uint32 control_init_bitmap(uint32 start_block, uint32 bit_qty)
{
  uint8* bitmap = NULL;
  uint32 bm_size = 0;

#ifdef FS_DEBUG_ON
  printf("In function control_init_bitmap: \n");
#endif

  bm_size = bit_qty / 8;

  if((bit_qty % 8) != 0)
    bm_size++;

  bitmap = malloc(bm_size);
  memset(bitmap, 0, bm_size);

  return coreio_write_block(bitmap, bm_size, start_block);

}

void control_init_inode_table(uint32 start_block, uint32 inode_qty)
{
  uint8* data = NULL;

#ifdef FS_DEBUG_ON
  printf("In function control_init_inode_table: \n");
#endif

  data = calloc(inode_qty, sizeof(inode));

  coreio_write_block(data, inode_qty * sizeof(inode), start_block);

  return;
}

uint8 control_modify_inode_bitmap(uint32 blk_id, bit_edit_mode m)
{
  uint8 data = 0;

#ifdef FS_DEBUG_ON
  printf("In function control_modify_inode_bitmap: \n");
#endif

  coreio_fseek(open_fs, (sb.inode_bitmap * BLOCK_SIZE) + (blk_id / 8), SEEK_SET);

  if(!coreio_fread(&data, sizeof(uint8), 1, open_fs))
    return -1;

  util_array_bit_alter(&data, blk_id, blk_id, 1, m);

  coreio_fseek(open_fs, -1, SEEK_CUR);

  coreio_fwrite(&data, sizeof(uint8), 1, open_fs);

  return 0;
}

uint8 control_modify_blk_bitmap(uint32 blk_id, bit_edit_mode m)
{
  uint8 data = 0;

#ifdef FS_DEBUG_ON
  printf("In function control_modify_blk_bitmap: \n");
#endif

  coreio_fseek(open_fs, (sb.block_table * BLOCK_SIZE) + (blk_id / 8), SEEK_SET);

  if(!coreio_fread(&data, sizeof(uint8), 1, open_fs))
    return -1;

  util_array_bit_alter(&data, blk_id, blk_id, 1, m);

  coreio_fseek(open_fs, -1, SEEK_CUR);

  coreio_fwrite(&data, sizeof(uint8), 1, open_fs);

  return 0;
}

uint32 control_find_free_block()
{
  uint8 mask = 0x01;
  uint8 cur_byte = 0;
  uint32 blk_cntr = sb.block_count / 8;
  uint32 i = 0;
  uint32 j = 0;
  uint32 k = 0;
  uint8* data = NULL;

#ifdef FS_DEBUG_ON
  printf("In function control_find_free_block: \n");
#endif

  blk_cntr = blk_cntr / BLOCK_SIZE;

  if(((sb.block_count / 8) % BLOCK_SIZE) != 0)
    blk_cntr++;

  data = malloc(BLOCK_SIZE);

  for(i = 0; i < blk_cntr; i++)
    {
      coreio_read_block(data, 1, sb.block_table + i);

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

void control_set_working_inode(uint32 inode_id)
{
  inode cur_inode = {0};

#ifdef FS_DEBUG_ON
  printf("In function control_set_working_inode: \n");
#endif

  cur_inode = inode_get_inode(inode_id);

  if(cur_inode.mode != IFDIR)
    {
#ifdef FS_DEBUG_ON
      printf("Not a valid inode directory. Exit.");
#endif
    }

  current_dir = cur_inode;

  return;
}

void control_create_directory_inode(inode target_inode)
{
  dir_entry entry = {0};

#ifdef FS_DEBUG_ON
  printf("In function control_create_directory_inode: \n");
#endif

  if(target_inode.mode == IFDIR)
    {
#ifdef FS_DEBUG_ON
      printf("Inode already a directory.\n");
#endif
      return;
    }

  if(target_inode.block_span != 0)
    {
#ifdef FS_DEBUG_ON
      printf("Inode contains data. Impossible to convert to directory.\n");
#endif
      return;
    }

  target_inode.mode = IFDIR;

  entry.inode = target_inode.inode_id;
  strcpy(entry.name, ".");

  control_add_directory_entry(target_inode, entry);

  return;
}

void control_add_file_to_directory(const char* name, uint32 target_inode)
{
  dir_entry entry = {0};
  inode cur_inode = {0};

#ifdef FS_DEBUG_ON
  printf("In function control_add_file_to_directory: \n");
#endif

  if(name == NULL)
    {
#ifdef FS_DEBUG_ON
  printf("Nullpointer received.\n");
#endif
  return;
    }

  cur_inode = inode_get_inode(target_inode);

  entry.inode = cur_inode.inode_id;
  strcpy(entry.name, name);

  control_add_directory_entry(current_dir, entry);

  if(cur_inode.mode == IFDIR)
    {
      entry.inode = current_dir.inode_id;
      strcpy(entry.name,"..");
      
      control_add_directory_entry(cur_inode, entry);
    }

  return;
}

int8 control_remove_file_from_directory(uint32 inode_id)
{
  uint8* data = NULL;
  dir_entry* entry = NULL;
  dir_entry tmp = {0};
  int i = 0;
  uint8 found_flag = 0;
  inode cur_inode = {0};

  cur_inode = inode_get_inode(inode_id);

  data = inodedata_read_block(cur_inode, 0);

  entry = (dir_entry*)data;

  for(i = 0; i < 16; i++)
    {
      if(entry->inode == inode_id)
	{
#ifdef FS_DEBUG_ON
	  printf("Found inode reference at offset %d\n", i);
#endif
	  memcpy(entry, &tmp, sizeof(dir_entry));

	  found_flag = 1;
	}
    }

  if(found_flag)
    {
#ifdef FS_DEBUG_ON
      printf("Erasing inode reference.\n");
#endif
      
      inodedata_write_block(data, BLOCK_SIZE, current_dir);
    }
  else
    {
#ifdef FS_DEBUG_ON
      printf("Reference not found.");
#endif
    }

  free(data);

  return 0;
}

static int8 control_add_directory_entry(inode target_inode, dir_entry entry)
{
  uint8* data = NULL;
  dir_entry* dpointer = NULL;
  uint32 i = 0;

  if(target_inode.mode != IFDIR)
    return -1;

  if(target_inode.block_span == 0)
    {
      data = calloc(1, BLOCK_SIZE);
      memcpy(data, &entry, sizeof(dir_entry));

      inodedata_write_block(data, BLOCK_SIZE, target_inode);
    }
  else
    {
      data = inodedata_read_block(target_inode, 0);
      dpointer = (dir_entry*)data;

      for(i = 0; i < 16; i++)
	{
	  if(dpointer->name[0] == 0)
	    {
	      memcpy(dpointer, &entry, sizeof(dir_entry));
	      break;
	    }

	  dpointer += sizeof(dir_entry);
	}

      inodedata_write_block(data, BLOCK_SIZE, target_inode);
    }

  free(data);

  return 0;
}
