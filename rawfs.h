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

// Main struct representing the superblock for the entire partition


raw_err rawfs_format(const uint8* file);

raw_err rawfs_open_device(const uint8* file);
raw_err rawfs_close_device();

uint32 rawfs_create_file(const uint8* path);
raw_err rawfs_delete_file(const uint8* path);
raw_err rawfs_read_data(const uint32 inode);
raw_err rawfs_write_data(uint32 inode, void* data, uint32 data_size);
raw_err rawfs_get_file_name(uint32 inode);
raw_err rawfs_get_file_inode(const uint8* path);

#endif
