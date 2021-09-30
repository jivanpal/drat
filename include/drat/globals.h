#ifndef DRAT_GLOBALS_H
#define DRAT_GLOBALS_H

#include <stdint.h>
#include <apfs/general.h>
#include <apfs/object.h>

/**
 * Data structure used to hold the values of Drat's global options.
 */
typedef struct {
    char*       program_name;
    char*       command_name;
    char*       container_path;
    
    // Need all numbers to be `int64_t` to work with `parse_number()`
    int64_t     max_xid;
    int64_t     block_size;
    int64_t     volume;

    char*       volume_name;
} global_options_t;

extern global_options_t globals;
void print_globals();

#endif // DRAT_GLOBALS_H
