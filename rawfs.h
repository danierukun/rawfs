#ifndef RAWFS_H
#define RAWFS_H

// Main block of the filesystem
/*
there are always 1024 free bytes at the beginning of the fs.
inodes:
point to the actual blocks of information. 12 first addresses point directly to data,
13 points to a block of pointers that each point to a block of data, 14 points to a block
with pointers to blocks with pointers and so on.
 */

#include <stdio.h>

#include "util.h"
#include "coreio.h"
#include "control.h"

uint32 rawfs_create_file(const uint8* name);
uint8** rawfs_get_file_list();
raw_err rawfs_delete_file(const uint8* name);
uint8* rawfs_get_working_directory();
raw_err rawfs_set_working_directory(uint32 inode_id);
raw_err rawfs_read_data(void* data, uint32 block_qty, uint32 block_offset,const uint8* filename);
raw_err rawfs_write_data(const uint8* name, const void* data, uint32 data_size);

#endif
