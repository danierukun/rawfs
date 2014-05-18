#include "rawfs.h"
#include "inodedata.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32 rawfs_get_filename_inode_id(const uint8* name);
static uint8* rawfs_get_inode_id_filename(uint32 inode_id);

// ------------------- implementation --------------------------

uint32 rawfs_create_file(const uint8* name)
{
  uint32 inode_id = 0;

#ifdef FS_DEBUG_ON
  printf("In function rawfs_create_file: \n");
#endif

  if(name == NULL)
    {
#ifdef FS_DEBUG_ON
      printf("Null string received. Exit.");
#endif
    return ERR_NULLPTR;
    }

  inode_id = inode_find_free_inode();

  if(inode_id == 0xFFFFFFFF)
    {
#ifdef FS_DEBUG_ON
      printf("No inodes available. Exit.");
#endif
      return ERR_SPACE;
    }

  inode_set_valid(inode_id);

  return SUCCESS;
}

uint8** rawfs_get_file_list()
{
  uint8* data = NULL; 
  uint8** names = NULL;
  int i = 0;
  dir_entry* entry = NULL;

  data = inodedata_read_block(current_dir, 0);

  names = calloc(1, 16);

  entry = (dir_entry*)data;

  for(i = 0; i < 16; i++)
    {
      names[i] = malloc(FILENAME_SIZE);
      strcpy(names[i], entry->name);
      entry += sizeof(dir_entry);
    }

  free(data);

    return names;
}

raw_err rawfs_delete_file(const uint8* name)
{
  uint32 inode_id = 0;
  inode cur_inode = {0};
  int i = 0;

  if((inode_id = rawfs_get_filename_inode_id(name)) == 0xFFFFFFFF)
    {
#ifdef FS_DEBUG_ON
      printf("No such file.\n");
#endif

      return ERR_NONEXIST;
    }

  cur_inode = inode_get_inode(inode_id);

  for(i = 0; i < cur_inode.block_span; i++)
    inodedata_deallocate_block(cur_inode, i);

  inode_set_invalid(inode_id);

  control_remove_file_from_directory(inode_id);

  return SUCCESS;
}

uint8* rawfs_get_working_directory()
{
  uint32 inode_id = 0;

  inode_id = rawfs_get_filename_inode_id("..");
  return rawfs_get_inode_id_filename(inode_id);
}

raw_err rawfs_set_working_directory(uint32 inode_id)
{
  current_dir = inode_get_inode(inode_id);

  return SUCCESS;
}

raw_err rawfs_read_data(void* data, uint32 block_qty, uint32 block_offset,const uint8* filename)
{
  uint32 inode_id = 0;
  uint8* inode_data = NULL;
  inode cur_inode = {0};
  int i = 0;

  if((inode_id = rawfs_get_filename_inode_id(filename)) == 0xFFFFFFFF)
    {
      return ERR_NONEXIST;
    }

  inode_id = rawfs_get_filename_inode_id(filename);
  cur_inode = inode_get_inode(inode_id);

  for(i = 0; i < block_qty; i++)
    {
      inode_data = inodedata_read_block(cur_inode, block_offset + i);
      memcpy(data, inode_data, BLOCK_SIZE);
      free(inode_data);
    }

  return SUCCESS;
}

raw_err rawfs_write_data(const uint8* name, const void* data, uint32 data_size)
{
  inode cur_inode = {0};
  uint32 inode_id = 0;

  if(name == NULL || data == NULL)
    {
#ifdef FS_DEBUG_ON
      printf("Null pointer received");
#endif
      return ERR_NULLPTR;
    }
  
  inode_id = rawfs_get_filename_inode_id(name);
  cur_inode = inode_get_inode(inode_id);

  if(inodedata_write_block(data, data_size, cur_inode) == 0)
    return SUCCESS;
  else
    return ERR_UNKNOWN;
}

//----------------- static functions --------------------------------

static uint32 rawfs_get_filename_inode_id(const uint8* name)
{
  uint8* data = NULL;
  int i = 0;
  dir_entry* entry = NULL;
  uint32 inode_id = 0;

  data = inodedata_read_block(current_dir, 0);
  entry = (dir_entry*)data;

  for(i = 0; i < 16; i++)
    {
      if(!strcmp(entry->name, name))
	{
	  inode_id = entry->inode;
	  return inode_id;
	}

      entry += sizeof(dir_entry);
    }

  free(data);

  return 0xFFFFFFFF;
}

static uint8* rawfs_get_inode_id_filename(uint32 inode_id)
{
  uint8* data = NULL;
  int i = 0;
  dir_entry* entry = NULL;
  uint8* name = NULL;

  data = inodedata_read_block(current_dir, 0);
  entry = (dir_entry*)data;

  for(i = 0; i < 16; i++)
    {
      if(entry->inode == inode_id)
	{
	  name = malloc(FILENAME_SIZE);
	  strcpy(name, entry->name);
	  break;
	}

      entry += sizeof(dir_entry);
    }

  free(data);

  return name;
}
