#include "rawfs.h"

#include <math.h>

// ------------------- implementation --------------------------

void rawfs_read_superblock()
{
  if(open_fs == NULL)
    return;

  fread(&sb, sizeof(superblock), 1, open_fs);

  return;
}

uint32 rawfs_create_file(const uint8* path)
{
}

raw_err rawfs_delete_file(const uint8* path)
{
}

raw_err rawfs_read_data(const uint32 inode)
{
}

raw_err rawfs_write_data(uint32 inode, void* data, uint32 data_size)
{
}

raw_err rawfs_get_file_inode(const uint8* path)
{
}
