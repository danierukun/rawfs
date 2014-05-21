#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rawfs.h"
#include "control.h"

int main(void)
{
  superblock* b = NULL;
  uint32 block_bitmap_blk_size = 0;
  uint32 inode_bitmap_blk_size = 0;
  uint32 used_blocks = 2;
  uint8* blk_array = NULL;

  if(coreio_open_device("./discoprueba.bin") != SUCCESS)
    {
      printf("Error opening file.\n");
      return -1;
    }

  b = control_construct_superblock(1);

  block_bitmap_blk_size = util_calculate_bitmap_block_span(b->block_count);
  inode_bitmap_blk_size = util_calculate_bitmap_block_span(b->inode_count);

  b->block_table = 2;
  b->inode_bitmap = block_bitmap_blk_size + 1;
  b->inode_table = b->inode_bitmap + inode_bitmap_blk_size + 1;

  used_blocks += control_init_bitmap(b->block_table, b->block_count);
  used_blocks += control_init_bitmap(b->inode_bitmap, b->inode_count);
  control_init_inode_table(b->inode_table, b->inode_count);

  used_blocks += (b->inode_count * sizeof(inode)) / BLOCK_SIZE;

  if(((b->inode_count * sizeof(inode)) % BLOCK_SIZE) != 0)
    used_blocks++;

  b->free_blocks = b->block_count - used_blocks;

  coreio_write_block(b, sizeof(superblock), 1);

  blk_array = malloc(used_blocks);
  
  memset(blk_array, 0xFF, used_blocks);

  coreio_fseek( open_fs, b->block_table * BLOCK_SIZE, SEEK_SET);

  coreio_fwrite(blk_array, used_blocks, 1, open_fs);

  //rawfs_create_file("root");

  coreio_close_device();

  free(b);
  free(blk_array);

  return 0;
}
