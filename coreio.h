#ifndef COREIO_H
#define COREIO_H

#define BLOCK_SIZE 1024

typedef enum
  {
    SUCCESS,
    ERR_UNKNOWN,
    ERR_NULLPTR,
    ERR_OPEN,
    ERR_IO,
    ERR_SPACE
  } raw_err;

uint32 coreio_read_block(void* data, uint32 block_qty, uint32 block_id);
uint32 coreio_write_block(const void* data, uint32 d_size, uint32 block_id);

#endif
