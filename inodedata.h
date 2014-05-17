#ifndef INODEDATA_H
#define INODEDATA_H

#include "inode.h"

uint8* inodedata_read_block(inode cur_inode, uint32 block_id);
int8 inodedata_write_block(void* data, uint32 data_size, inode cur_inode);
int8 inodedata_deallocate_block(inode cur_inode, uint32 block_id);
uint32 inodedata_traverse_indirect_blocks(inode cur_inode, offset_container offset);

#endif
