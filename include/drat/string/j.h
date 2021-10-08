#ifndef DRAT_STRING_J_H
#define DRAT_STRING_J_H

#include <stdbool.h>
#include <stdint.h>

#include <apfs/dstream.h>
#include <apfs/j.h>

char* j_key_type_to_string(uint8_t j_key_type);
void print_j_key(j_key_t* key);
void print_j_inode_key(j_inode_key_t* key);

char* get_j_inode_type(j_inode_val_t* val);
char* j_inode_mode_to_string(apfs_mode_t mode);
char* get_j_inode_internal_flags_string(uint64_t internal_flags);
char* get_j_inode_bsd_flags_string(uint32_t bsd_flags);

char* get_item_name(uint8_t *xfields, uint16_t val_len);
void print_j_inode_info(j_inode_val_t* val, uint16_t val_len);
void print_j_inode_val(j_inode_val_t* val, uint16_t val_len);
void print_j_file_extent_key(j_file_extent_key_t* key);
void print_j_file_extent_val(j_file_extent_val_t* val);
void print_j_drec_hashed_key(j_drec_hashed_key_t* key);

char* drec_val_to_type_string(j_drec_val_t* val);
char* drec_val_to_short_type_string(j_drec_val_t* val);
void print_j_drec_val(j_drec_val_t* val, uint16_t val_len);

void print_j_dir_stats_val(j_dir_stats_val_t* val);

#endif // DRAT_STRING_J_H
