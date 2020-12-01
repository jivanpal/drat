#ifndef APFS_STRING_FS_H
#define APFS_STRING_FS_H

#include <time.h>

#include "../struct/fs.h"
#include "object.h"

char* get_apfs_features_string(apfs_superblock_t* apsb);
char* get_apfs_readonly_compatible_features_string(apfs_superblock_t* apsb);
char* get_apfs_incompatible_features_string(apfs_superblock_t* apsb);
char* get_apfs_fs_flags_string(apfs_superblock_t* apsb);
char* get_apfs_role_string(apfs_superblock_t* apsb);
void print_apfs_superblock(apfs_superblock_t* apsb);

#endif // APFS_STRING_FS_H
