/**
 * Functions that generate nicely formatted strings representing data found in
 * the checkpoint area. This includes instances of `nx_superblock_t`,
 * `checkpoint_map_phys_t`, and `checkpoint_mapping_t`.
 */

#include "../struct/nx.h"

#include "object.h"

/**
 * Print a nicely formatted string describing the data contained in a container
 * superblock, including the data in its header.
 */
print_nxsb_info(nx_superblock_t* nxsb) {
    print_obj_hdr_info(nxsb); // `nxsb` is equivalent to `&(nxsb->nx_o)`.

    char magic_string[] = {
        (char)nxsb->nx_magic,
        (char)(nxsb->nx_magic >> 8),
        (char)(nxsb->nx_magic >> 16),
        (char)(nxsb->nx_magic >> 24),
        '\0'
    };
    printf("Magic string:       %s\n",          magic_string);
    printf("Block size:         %lu bytes\n",   nxsb->nx_block_size);
    printf("Block count:        %lu\n",         nxsb->nx_block_count);
    
    // TODO: print features fields: `nx_features`, `nx_readonly_compatible_features`, `nx_incompatible_features`.
    
    printf("UUID:               0x%016llx%016llx\n",    *(uint64_t*)(&(nxsb->nx_uuid[8])),  *(uint64_t*)(&(nxsb->nx_uuid[0])));
    printf("Next OID:           0x%016llx\n",   nxsb->nx_next_oid);
    printf("Next XID:           0x%016llx\n",   nxsb->nx_next_xid);

    // TODO: Maybe print `xp_desc` and `xp_data` fields.

    printf("Space manager OID:  0x%016llx\n",   nxsb->nx_spaceman_oid);
    printf("Object map OID:     0x%016llx\n",   nxsb->nx_omap_oid);
}
