#include <string.h>
#include <stdio.h>
// #include <stdio.h>

#include "util.h"


/*
Toma los primeros 4 bytes de un array y los encadena en un int. No esta
protegida contra errores de nullpointer
 */
uint32 util_char_array_to_int(const uint8* array)
{
#ifdef FS_DEBUG_ON
  printf("In function util_array_bit_alter: \n");
#endif

  return (uint32)((array[0] << 24) | (array[1] << 16) | (array[2] << 8) | array[3]);
}

uint32 util_array_bit_alter(void* data_array, uint32 start_bit, uint32 end_bit, uint32 array_size, bit_edit_mode m)
{
  uint8 first_byte_mask = 0;
  uint8 last_byte_mask = 0;
  uint8 data_fill = 0;
  uint8* data = (uint8*)data_array;

#ifdef FS_DEBUG_ON
  printf("In function util_array_bit_alter: \n");
#endif

  switch(m)
    {
    case BIT_CLEAR:
      first_byte_mask = 0xFF << (8 - (start_bit % 8));
      last_byte_mask = 0xFF >> end_bit % 8;
      data_fill = 0;
      data[0] = data[0] & first_byte_mask;
      data[array_size - 1] = data[array_size - 1] & last_byte_mask;
      break;

    case BIT_SET:
      first_byte_mask = 0xFF >> start_bit % 8;
      last_byte_mask = 0xFF << (8 - (end_bit % 8));
      data_fill = 0xFF;
      data[0] = data[0] | first_byte_mask;
      data[array_size - 1] = data[array_size - 1] | last_byte_mask;
      break;

    default:
      return -1;
    }

  //  printf("first mask: %x\n", first_byte_mask);
  //  printf("last mask: %x\n", last_byte_mask);

  if(array_size > 2)
    memset(data + 1, data_fill, array_size - 2);

  return array_size;

}

uint32 util_calculate_bitmap_block_span(uint32 bitmap_bit_size)
{
  uint32 res = 0;

  res = bitmap_bit_size / 8;

  if((bitmap_bit_size % 8) != 0)
    res++;

  return res;
}
