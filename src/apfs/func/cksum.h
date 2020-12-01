#ifndef APFS_FUNC_CKSUM_H
#define APFS_FUNC_CKSUM_H

#include <stdint.h>
#include <stdbool.h>

#include "../io.h"

uint64_t fletcher_cksum(uint32_t* block, bool compute);
uint64_t compute_block_cksum(uint32_t* block);
bool is_cksum_valid(uint32_t* block);

#endif // APFS_FUNC_CKSUM_H
