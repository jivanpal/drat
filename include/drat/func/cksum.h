#ifndef DRAT_FUNC_CKSUM_H
#define DRAT_FUNC_CKSUM_H

#include <stdbool.h>
#include <stdint.h>

uint64_t fletcher_cksum(uint32_t* block, bool compute);
uint64_t compute_block_cksum(uint32_t* block);
bool is_cksum_valid(uint32_t* block);

#endif // DRAT_FUNC_CKSUM_H
