#include "rawfs.h"
#include "controlstruct.h"

#include <math.h>

// ------------------- implementation --------------------------

raw_err rawfs_open_device(const uint8* file)
{
  if(file == NULL)
    return ERR_NULLPTR;

  if(open_fs != NULL)
    fclose(open_fs);

  if((open_fs = fopen(file, "w+b")) == NULL)
    return ERR_IO;

  return SUCCESS;
}

raw_err rawfs_close_device()
{
  if(open_fs == NULL)
    return SUCCESS;

  fclose(open_fs);
  open_fs = NULL;
  return SUCCESS;
}

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
