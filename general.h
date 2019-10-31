/**
 * Structures and related item as defined in
 * §2 "General-Purpose Types"
 */

#include <stdint.h>

typedef int64_t     paddr_t;

typedef struct prange {
    paddr_t     pr_start_paddr;
    uint64_t    pr_block_count;
} prange_t;

typedef unsigned char   uuid_t[16];
