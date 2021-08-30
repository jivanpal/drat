#ifndef DRAT_ARGP_H
#define DRAT_ARGP_H

#include <stdint.h>
#include <stdbool.h>
#include <argp.h>

#include <apfs/object.h>

#include <drat/commands.h>

#define ARGP_KEY_CONTAINER      (-1)
#define ARGP_KEY_BLOCK_SIZE     (-2)
#define ARGP_KEY_VOLUME         (-3)
#define ARGP_KEY_VOLUME_NAME    (-4)
#define ARGP_KEY_MAX_XID        (-5)

/**
 * Data structure used to hold the values of Drat's global options.
 */
typedef struct {
    char*       program_name;
    xid_t       max_xid;
    uint32_t    block_size;
    int8_t      volume_index;
    char*       volume_name;
    char*       container_path;
} global_options_t;

/**
 * Data structure used to hold values in relation to parsing the command from
 * the command-line arguments.
 */
typedef struct {
    bool                seen;
    char*               name;
    command_function*   function;
} command_info_t;

extern global_options_t globals;
extern const struct argp_option argp_options_globals[];
error_t argp_parser_command(int key, char* arg, struct argp_state* state);
error_t argp_parser_globals(int key, char* arg, struct argp_state* state);
extern const struct argp argp_command;
extern const struct argp argp_globals;

#endif // DRAT_ARGP_H
