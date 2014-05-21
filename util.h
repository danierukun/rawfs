#ifndef UTIL_H
#define UTIL_H

#define TRUE 1
#define FALSE 0

typedef unsigned char uint8;
typedef char int8;
typedef unsigned short uint16;
typedef short int16;
typedef unsigned int uint32;
typedef int int32;

typedef enum
  {
    BIT_CLEAR,
    BIT_SET
  } bit_edit_mode;

/*
 * Alters an array at bit level to set or clear subsequent bits.
 */

uint32 util_char_array_to_int(const uint8* array);
uint32 util_array_bit_alter(void* data_array, uint32 start_bit, uint32 end_bit,uint32 array_size, bit_edit_mode m);
uint32 util_calculate_bitmap_block_span(uint32 bitmap_bit_size);

#endif
