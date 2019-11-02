/**
 * Functions related to Fletcher-64 checksums.
 */

/**
 * Compute or validate the checksum of a given APFS block. This is a helper
 * function for `compute_block_cksum()` and `is_cksum_valid()`.
 * 
 * block:       A pointer to the raw APFS block data. This pointer should point
 *          to at least `nx_block_size` bytes (typically 4096 bytes) of data.
 * compute:     Boolean value. If non-zero, then compute the checksum of the
 *          block, treating the first 64 bits of the block (where the checksum
 *          is stored) as zero. If zero, then validate checksum against the
 *          checksum stored in the block header.
 * 
 * RETURN VALUE:    If `compute` is true, return the computed checksum.
 *              If `compute` is false, return zero if the checksum validates
 *              successfully, and non-zero if it fails to do so.
 */
uint64_t fletcher_cksum(uint32_t* block, char compute) {
    int num_words = nx_block_size / 4;  // Using 32-bit words.
    uint32_t modulus = ~0;  // all ones; = 2^32 - 1

    // These are 32-bit values, but we need at least 33 bits of memory for each
    // of these in order to compute them, as values can overflow during
    // summation before the modulus is taken.
    // We use the next best thing: 64 bits.
    uint64_t simple_sum = 0;
    uint64_t second_sum = 0;

    // NOTE: When computing the checksum, `i = 2` is used here since we treat
    // the first 64 bits of the block as zero. When validating the checksum, we
    // compute the traditional Fletcher-64 checksum of the entire block.
    for (int i = (compute ? 2 : 0); i < num_words; i++) {
        simple_sum = (simple_sum + block[i])    % modulus;
        second_sum = (second_sum + simple_sum)  % modulus;
    }

    /**
     * APFS uses a variant of the traditional Flecther-64 checksum.
     * See: https://blog.cugu.eu/post/apfs/#checksum
     * 
     * In particular:
     * 
     * - Instead of `simple_sum`, APFS uses `c1`, which (in number theory lingo)
     * is the remainder of `- (simple_sum + second_sum)` modulo `modulus`.
     * 
     * - Instead of `second_sum`, APFS uses `c2`, which (in number theory lingo)
     * is the remainder of  `- (simple_sum + c1)` modulo `modulus`. However, it
     * turns out that this `c2` value always just equals `second_sum` itself,
     * so there is no need to compute `c2`.
     * 
     * This variant of Fletcher-64 means that, when validating against the
     * stored checksum, we just compute the traditional Fletcher-64 checksum of
     * the whole block (including the stored checksum) and the computed value
     * should be zero if the stored checksum is correct.
     * 
     * Here, if we are computing the checksum rather than validating the block
     * against the stored checksum, we compute `c1` and just store it in the
     * variable `simple_sum`.
     */
    // if (compute) {
        simple_sum = modulus - ((simple_sum + second_sum) % modulus);
    // }

    return (second_sum << 32) | simple_sum;
}

/**
 * Get/compute the checksum of a given APFS block, treating the first 64 bits
 * of the block (the location where the checksum is usually stored) as zero.
 * 
 * block:   A pointer to the raw APFS block data. This pointer should point to
 *      at least `nx_block_size` bytes (typically 4096 bytes) of data.
 * 
 * RETURN VALUE:    The computed checksum.
 */
uint64_t compute_block_cksum(uint32_t* block) {
    return fletcher_cksum(block, 1);
}

/**
 * Determine whether a given APFS block has a valid checksum.
 */
char is_cksum_valid(uint32_t* block) {
    // TODO: The "simple" implementation doesn't appear to work.
    // return fletcher_cksum(block, 0) == 0;

    // The following gives the correct result.
    return compute_block_cksum(block) == *(uint64_t*)block;
}