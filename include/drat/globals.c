#include "globals.h"

#include <stdio.h>
#include <inttypes.h>

// Set default global option values
global_options_t globals = {
    NULL,   // program_name, set in main()
    NULL,   // command_name, set in main() via Argp
    NULL,   // container_path
    -1,     // max_xid
    0,      // block_size (0 = auto)
    0,      // volume (0 = container, not any volume)
    NULL,   // volume_name
};

/**
 * Print the value of the global options. Used for debugging.
 */
void print_globals() {
    fprintf(
        stderr,

        "\n"
        "program name   = %s\n"
        "container path = %s\n"
        "max xid        = %#"PRIx64"\n"
        "block size     = %"PRId64" bytes\n"
        "volume index   = %"PRId64"\n"
        "volume name    = %s\n",

        globals.program_name,
        globals.container_path,
        globals.max_xid,
        globals.block_size,
        globals.volume,
        globals.volume_name
    );
}
