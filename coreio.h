#ifndef COREIO_H
#define COREIO_H

#include <stdio.h>
#include "util.h"

#define BLOCK_SIZE 1024

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

typedef enum
  {
    SUCCESS,
    ERR_UNKNOWN,
    ERR_NULLPTR,
    ERR_OPEN,
    ERR_IO,
    ERR_SPACE
  } raw_err;

FILE* open_fs;
superblock sb = {0};

raw_err coreio_open_device(const uint8* file);
raw_err coreio_close_device();
uint32 coreio_read_block(void* data, uint32 block_qty, uint32 block_id);
uint32 coreio_write_block(const void* data, uint32 d_size, uint32 block_id);
uint32 coreio_fread(void* ptr, uint32 size, uint32 nmemb, FILE* stream);
uint32 coreio_fwrite(const void* ptr, uint32 size, uint32 nmemb, FILE* stream);
uint32 coreio_fseek(FILE* stream, long offset, uint32 whence);

#endif
