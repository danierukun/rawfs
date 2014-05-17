#ifndef INODE_H
#define INODE_H

#include "util.h"

#define BLOCK_INODE_RATIO 64

typedef struct
{
  uint32 inode_id; // Stores the id of the inode, must reflect inode bitmap
  uint8 mode; // defines if inode is file or directory
  uint32 block_span; // number of blocks for this file
  double file_size; // size in bytes of file
  uint32 data_blocks[15]; // Pointers to data, 1-12 direct, 13 first indirect, 14 second indirect...
} inode;

typedef struct
{
  uint32 inode;  // inode asociated to this directory
  uint16 rec_len;  // displacement to next entry from start of current one
  uint8 name_len;  // length of the name
  uint8* name; // the actual name, in ascii
} dir_entry; // Special structure to be stored inside a inode

typedef struct
{
  uint8* data_blk_array; // Array pointing to the data blocks
  uint32 data_blk_qty; // contains the number of blocks in the array
} data_container;

typedef struct
{
  uint8 indirection_lvl; // level of indirection, can be -1, 0, 1, 2 or 3
  uint32 indirection_index[4]; // indexes to find the data blocks
} offset_container;

typedef enum
  {
    IFREG,
    IFDIR,
    IFINVALID
  } inode_mode;

inode inode_get_inode(uint32 inode_id);
uint8 inode_is_valid(uint32 inode_id);
int8 inode_set_valid(uint32 inode_id);
uint8 inode_set_invalid(uint32 inode_id);

#endif
