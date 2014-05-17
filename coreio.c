#include "coreio.h"
#include <stdio.h>

raw_err coreio_open_device(const uint8* file)
{
  if(file == NULL)
    return ERR_NULLPTR;

  if(open_fs != NULL)
    fclose(open_fs);

  if((open_fs = fopen(file, "w+b")) == NULL)
    return ERR_IO;

  return SUCCESS;
}

raw_err coreio_close_device()
{
  if(open_fs == NULL)
    return SUCCESS;

  fclose(open_fs);
  open_fs = NULL;
  return SUCCESS;
}

uint32 coreio_read_block(void* data, uint32 block_qty, uint32 block_id)
{
#ifdef FS_DEBUG_ON
  printf("In function coreio_read_block: \n");
#endif

  coreio_fseek(open_fs, block_id * BLOCK_SIZE, SEEK_SET);
  return (coreio_fread(data, BLOCK_SIZE, block_qty * BLOCK_SIZE, open_fs) / (BLOCK_SIZE + 1));
}

uint32 coreio_write_block(const void* data, uint32 d_size, uint32 block_id)
{
  uint32 written_blks = 0;

#ifdef FS_DEBUG_ON
  printf("In function coreio_write_block: \n");
#endif

  coreio_fseek(open_fs, block_id * BLOCK_SIZE, SEEK_SET);
  written_blks = coreio_fwrite(data, d_size, 1, open_fs) / (BLOCK_SIZE + 1);
  return written_blks;
}

uint32 coreio_fread(void* ptr, uint32 size, uint32 nmemb, FILE* stream)
{
  return fread(ptr, size, nmemb, stream);
}

uint32 coreio_fwrite(const void* ptr, uint32 size, uint32 nmemb, FILE* stream)
{
  return fwrite(ptr, size, nmemb, stream);
}

uint32 coreio_fseek(FILE* stream, long offset, uint32 whence)
{
  return fseek(stream, offset, whence);
}
