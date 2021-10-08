#include "dstream.h"

#include <inttypes.h>
#include <stdio.h>

void print_j_dstream(j_dstream_t* dstream) {
    printf(
        "Size:                  %"PRIu64" bytes\n"
        "Allocated size:        %"PRIu64" bytes\n"
        "Default crypto ID:     %#"PRIx64"\n"
        "Total bytes written:   %"PRIu64" bytes\n"
        "Total bytes read:      %"PRIu64" bytes\n",

        dstream->size,
        dstream->alloced_size,
        dstream->default_crypto_id,
        dstream->total_bytes_written,
        dstream->total_bytes_read
    );
}
