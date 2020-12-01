#ifndef APFS_STRING_NX_H
#define APFS_STRING_NX_H

#include "../struct/nx.h"
#include "object.h"

char* get_nx_features_string(nx_superblock_t* nxsb);
char* get_nx_readonly_compatible_features_string(nx_superblock_t* nxsb);
char* get_nx_incompatible_features_string(nx_superblock_t* nxsb);
char* get_nx_flags_string(nx_superblock_t* nxsb);
void print_nx_superblock(nx_superblock_t* nxsb);
void print_checkpoint_mapping(checkpoint_mapping_t* cpm);
char* get_cpm_flags_string(checkpoint_map_phys_t* cpm);
void print_checkpoint_map_phys(checkpoint_map_phys_t* cpm);
void print_checkpoint_map_phys_mappings(checkpoint_map_phys_t* cpm);

#endif // APFS_STRING_NX_H
