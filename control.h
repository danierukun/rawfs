#ifndef CONTROLSTRUCT_H
#define CONTROLSTRUCT_H

#include <stdio.h>

#include "coreio.h"

#define ROOT_INODE_NO 2

typedef struct
{
  uint32 block_size;   // Size of the block unit in KiB
  uint32 block_count;  // total number of blocks in fileystem
  uint32 free_blocks;  // number of free blocks in filesystem
  uint32 inode_count;  // Total quantity of inodes
  uint32 free_inodes;  // Number of free inodes
  uint32 block_table;  // Start block of the block bitmap
  uint32 inode_bitmap; // Start block of the inode bitmap
  uint32 inode_table;  // start block of the inode table
} superblock;

FILE* open_fs;
superblock sb = {0};
inode current_dir = {0};

#endif
