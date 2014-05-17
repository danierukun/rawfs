#include "inode.h"

static offset_container inode_calculate_offsets(uint32 block_id)
{
  offset_container c = {0};
  uint32 pointer_size = BLOCK_SIZE / 4;

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

static inode inode_get_inode(uint32 inode_id)
{
  inode tmp = {0};

  if(open_fs == NULL)
    return 0;

  fseek(open_fs, (sb.inode_table * BLOCK_SIZE) + (inode_id * sizeof(inode)), SEEK_SET);

  fread(&tmp, sizeof(inode), 1, open_fs);

  return tmp;
}

uint8* rawfs_inode_read_block(inode cur_inode, uint32 block_id)
{
  offset_container offset = {0};
  uint32 addr = 0;
  int i = 0;
  uint8* data = NULL;
  uint32 block_addr = 0;

  offset = inode_calculate_offsets(block_id);

  if(offset.indirection_lvl == -1)
    return;

  data = malloc(BLOCK_SIZE);
    
  coreio_read_block(data, 1, cur_inode.data_blocks[offset.indirection_index[0]]);

  for(i = 1; i < offset.indirection_lvl; i++)
    {
      block_addr = (uint32)((data[offset.indirection_index[i]] << 24) | ((data[offset.indirection_index[i]] + 1) << 16) | ((data[offset.indirection_index[i]] + 2) << 8) | (data[offset.indirection_index[i]] + 3));
      
      coreio_read_block(data, 1, block_addr);
    }
  
  return data;
}

int8 inode_write_block(void* data, uint32 data_size, inode cur_inode)
{
  uint32 block_addr = 0;
  offset_container offset = {0};

  if(cur_inode.block_span >= (pow(BLOCK_SIZE / 4, 3) + pow(BLOCK_SIZE / 4, 2) + (BLOCK_SIZE / 4) + 12))
    return -1;

  while((data_size - BLOCK_SIZE) >= BLOCK_SIZE)
    {
      block_addr = control_find_free_block();
      
      if(block_addr == -1)
	return -1;
      
      offset = inode_calculate_offset(cur_inode.block_span);
      
      // TODO indirection write

      coreio_write_block(data, BLOCK_SIZE, );

      data_size -= BLOCK_SIZE;
    }
}

static uint8 rawfs_inode_is_valid(uint32 inode_id)
{
  uint8 data = 0;
  uint8 mask = 0x01;

  if(open_fs == NULL)
    return -1;

  fseek(open_fs, (sb.inode_bitmap * BLOCK_SIZE) + (inode_id / 8), SEEK_SET);

  fread(&data, sizeof(uint8), 1, open_fs);

  mask = mask << inode_id % 8;

  data = data & mask;

  if(data)
    return 1;
  else
    return 0;
}
