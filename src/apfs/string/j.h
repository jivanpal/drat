#ifndef APFS_STRING_J_H
#define APFS_STRING_J_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

/**
 * Resolve a missing <sys/stat.h> definition in Xcode Command Line Tools
 * for macOS Mojave. See: https://github.com/jivanpal/apfs-tools/issues/1
 **/
#ifndef SF_DATALESS
#define SF_DATALESS 0x40000000
#endif

#include "../struct/j.h"
#include "../struct/dstream.h"
#include "../struct/xf.h"

char* j_key_type_to_string(uint8_t j_key_type);
void print_j_key(j_key_t* key);
void print_j_inode_key(j_inode_key_t* key);

char* j_inode_mode_to_string(mode_t mode);
char* get_j_inode_internal_flags_string(uint64_t internal_flags);
char* get_j_inode_bsd_flags_string(uint32_t bsd_flags);

void print_j_inode_val(j_inode_val_t* val, bool has_xfields);
void print_j_file_extent_key(j_file_extent_key_t* key);
void print_j_file_extent_val(j_file_extent_val_t* val);
void print_j_drec_hashed_key(j_drec_hashed_key_t* key);

char* drec_val_to_type_string(j_drec_val_t* val);
char* drec_val_to_short_type_string(j_drec_val_t* val);
void print_j_drec_val(j_drec_val_t* val, bool has_xfields);

#endif // APFS_STRING_J_H
