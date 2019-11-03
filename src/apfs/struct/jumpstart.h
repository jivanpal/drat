/**
 * Structures and related items as defined in
 * §4 "EFI Jumpstart"
 */

#ifndef APFS_STRUCT_JUMPSTART_H
#define APFS_STRUCT_JUMPSTART_H

#include "general.h"    // for `prange_t`
#include "object.h"     // for `obj_phys_t`

/** `nx_efi_jumpstart_t` **/

typedef struct nx_efi_jumpstart {
    obj_phys_t  nej_o;
    uint32_t    nej_magic;
    uint32_t    nej_version;
    uint32_t    nej_efi_file_len;
    uint32_t    nej_num_extents;
    uint64_t    nej_reserved[16];
    prange_t    nej_rec_extents[];
} nx_efi_jumpstart_t;

#define NX_EFI_JUMPSTART_MAGIC      'RDSJ'
#define NX_EFI_JUMPSTART_VERSION    1

/** Partition UUIDs **/

#define APFS_GPT_PARTITION_UUID     "7C3457EF-0000-11AA-AA11-00306543ECAC"

#endif // APFS_STRUCT_JUMPSTART_H
