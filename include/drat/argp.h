#ifndef DRAT_ARGP_H
#define DRAT_ARGP_H

#include <stdint.h>
#include <stdbool.h>
#include <argp.h>

#include <apfs/object.h>

#include <drat/commands.h>

#define DRAT_ARG_KEY_CONTAINER      (-1)
#define DRAT_ARG_KEY_BLOCK_SIZE     (-2)
#define DRAT_ARG_KEY_VOLUME         (-3)
#define DRAT_ARG_KEY_VOLUME_NAME    (-4)
#define DRAT_ARG_KEY_MAX_XID        (-5)

#define DRAT_ARG_ERR_NO_CONTAINER       (-1)
#define DRAT_ARG_ERR_INVALID_BLOCK_SIZE (-2)
#define DRAT_ARG_ERR_INVALID_VOLUME     (-3)
#define DRAT_ARG_ERR_INVALID_MAX_XID    (-4)

void print_arg_parse_error();
bool print_global_args_error(int error);

extern const struct argp_option argp_options_globals[];
error_t argp_parser_command(int key, char* arg, struct argp_state* state);
error_t argp_parser_globals(int key, char* arg, struct argp_state* state);
extern const struct argp argp_command;
extern const struct argp argp_globals;

#endif // DRAT_ARGP_H
