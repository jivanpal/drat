#ifndef DRAT_GLOBALS_H
#define DRAT_GLOBALS_H

#include <stdint.h>
#include <apfs/object.h>

/**
 * Data structure used to hold the values of Drat's global options.
 */
typedef struct {
    char*       program_name;
    char*       command_name;
    char*       container_path;
    xid_t       max_xid;
    uint32_t    block_size;
    int8_t      volume;
    char*       volume_name;
} global_options_t;

extern global_options_t globals;
void print_globals();

#endif // DRAT_GLOBALS_H
