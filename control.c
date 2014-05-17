#include "control.h"

superblock* control_construct_superblock(uint32 )
{
  superblock* s = NULL;
  int32 byte_qty = 0;

#ifdef FS_DEBUG_ON
  printf("In function control_construct_superblock: \n");
#endif

  if(open_fs == NULL)
    return;

  s = malloc(sizeof(superblock));

  fseek(open_fs,0,SEEK_END);
  byte_qty = ftell(open_fs);
  rewind(open_fs);

  s->block_size = BLOCK_SIZE;
  s->block_count = byte_qty / BLOCK_SIZE;
  s->free_blocks = s->block_count;
  s->inode_count = s->block_count / BLOCK_INODE_RATIO;
  s->free_inodes = s->inode_count;
}

void control_init_bitmap(uint32 start_block, uint32 bitmap_size)
{
  uint8* bitmap = NULL;
  uint32 bm_size = 0;

#ifdef FS_DEBUG_ON
  printf("In function control_init_bitmap: \n");
#endif

  bm_size = bitmap_size / 8;

  if((bitmap_size % 8) != 0)
    bm_size++;

  bitmap = malloc(bm_size);
  memset(bitmap, 0, bm_size);

  coreio_write_block(bitmap, bm_size, start_block);

}

uint8 control_modify_inode_bitmap(uint32 blk_id, bit_edit_mode m)
{
  uint8 data = 0;

#ifdef FS_DEBUG_ON
  printf("In function control_modify_inode_bitmap: \n");
#endif

  fseek(open_fs, (sb.inode_bitmap * BLOCK_SIZE) + (blk_id / 8), SEEK_SET);

  if(!fread(&data, sizeof(uint8), 1, open_fs))
    return -1;

  util_array_bit_alter(&data, blk_id, blk_id, 1, m);

  fseek(open_fs, -1, SEEK_CUR);

  fwrite(&data, sizeof(uint8), 1, open_fs);

  return 0;
}

uint8 control_modify_blk_bitmap(uint32 blk_id, bit_edit_mode m)
{
  uint8 data = 0;

#ifdef FS_DEBUG_ON
  printf("In function control_modify_blk_bitmap: \n");
#endif

  fseek(open_fs, (sb.block_table * BLOCK_SIZE) + (blk_id / 8), SEEK_SET);

  if(!fread(&data, sizeof(uint8), 1, open_fs))
    return -1;

  util_array_bit_alter(&data, blk_id, blk_id, 1, m);

  fseek(open_fs, -1, SEEK_CUR);

  fwrite(&data, sizeof(uint8), 1, open_fs);

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
