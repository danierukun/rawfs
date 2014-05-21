#ifndef CONTROLSTRUCT_H
#define CONTROLSTRUCT_H

#include <stdio.h>

#include "coreio.h"
#include "inode.h"

#define ROOT_INODE_NO 2

extern inode current_dir;

superblock* control_construct_superblock(uint32 block);
uint32 control_init_bitmap(uint32 start_block, uint32 bit_qty);
void control_init_inode_table(uint32 start_block, uint32 inode_qty);
uint8 control_modify_inode_bitmap(uint32 blk_id, bit_edit_mode m);
uint8 control_modify_blk_bitmap(uint32 blk_id, bit_edit_mode m);
uint32 control_find_free_block();
void control_set_working_inode(uint32 inode);
void control_create_directory_inode(inode target_inode);
void control_add_file_to_directory(const char* name, uint32 target_inode);
int8 control_remove_file_from_directory(uint32 inode_id);


#endif
