#include "argp.h"

#include <inttypes.h>
#include <assert.h>

#include <drat/strings.h>
#include <drat/globals.h>

const char* argp_program_version = VERSION_STRING;
const char* argp_program_bug_address = URL_STRING;

void print_arg_parse_error() {
    fprintf(
        stderr,
        "%s: ERROR: Failed to parse arguments. This should never happen; please file a bug at " URL_STRING "\n",
        globals.command_name
    );
}

/**
 * Print an error message on `stderr` corresponding to the error code `error`
 * returned by the Argp parser `argp_global`, and return `true`; or if `error`
 * is not such an error code, print nothing and return `false`.
 */
bool print_global_args_error(int error) {
    switch (error) {
        case DRAT_ARG_ERR_NO_CONTAINER:
            fprintf(stderr, "%s: option `--container` is mandatory.\n", globals.program_name);
            break;
        case DRAT_ARG_ERR_INVALID_BLOCK_SIZE:
            fprintf(
                stderr,
                "%s: option `--block-size` has invalid value; must be `auto` or"
                " a non-negative integer expressed in decimal or in hexdecimal"
                " with the prefix `0x`.\n",
                globals.program_name
            );
            break;
        case DRAT_ARG_ERR_INVALID_VOLUME:
            fprintf(
                stderr,
                "%s: option `--volume` has invalid value; must be a non-negative decimal integer.\n",
                globals.program_name
            );
            break;
        case DRAT_ARG_ERR_INVALID_MAX_XID:
            fprintf(
                stderr,
                "%s: option `--max-xid` has invalid value; must be an integer"
                " expressed in decimal or in hexadecimal with the prefix `0x`.\n",
                globals.program_name
            );
            break;
        default:
            return false;
    }

    return true;
}

/**
 * Argp options structure defining Drat's global options.
 */
const struct argp_option argp_options_globals[] = {
    //  char* name,     int key,                    DRAT_ARG_KEY arg,  int flags,  char* doc
    { "container",      DRAT_ARG_KEY_CONTAINER,     "path",     0,          "APFS container" },
    { "block-size",     DRAT_ARG_KEY_BLOCK_SIZE,    "size",     0,          "APFS block size in bytes" },
    { "volume",         DRAT_ARG_KEY_VOLUME,        "index",    0,          "APFS volume index" },
    { "volume-name",    DRAT_ARG_KEY_VOLUME_NAME,   "name",     0,          "APFS volume name" },
    { "max-xid",        DRAT_ARG_KEY_MAX_XID,       "xid",      0,          "Maximum transaction ID" },
    {0}
};

/**
 * Argp parser function that handles Drat's global options.
 */
error_t argp_parser_globals(int key, char* arg, struct argp_state* state) {
    switch (key) {
        // Drat argument keys

        case DRAT_ARG_KEY_BLOCK_SIZE: {
            if (strcmp(arg, "auto") == 0) {
                globals.block_size = 0;
                break;
            }
            char* endptr = NULL;
            uintmax_t value = strtoumax(arg, &endptr, 0);
            if (*arg == '\0' || *endptr != '\0') {
                return DRAT_ARG_ERR_INVALID_BLOCK_SIZE;
            }
            globals.block_size = value;
        } break;

        case DRAT_ARG_KEY_VOLUME: {
            char* endptr = NULL;
            uintmax_t value = strtoumax(arg, &endptr, 10);
            if (*arg == '\0' || *endptr != '\0') {
                return DRAT_ARG_ERR_INVALID_VOLUME;
            }
            globals.volume = value;
        } break;

        case DRAT_ARG_KEY_MAX_XID: {
            char* endptr = NULL;
            intmax_t value = strtoimax(arg, &endptr, 0);
            if (*arg == '\0' || *endptr != '\0') {
                return DRAT_ARG_ERR_INVALID_MAX_XID;
            }
            globals.max_xid = value;
        } break;

        case DRAT_ARG_KEY_CONTAINER:
            globals.container_path = arg;
            break;

        case DRAT_ARG_KEY_VOLUME_NAME:
            globals.volume_name = arg;
            break;

        // Argp special keys

        case ARGP_KEY_ARG:
            if (state->arg_num == 0) {
                break;  // This arg is the Drat command; skip.
            }
            // This is some other non-option argument; pass to parent parser.
            return ARGP_ERR_UNKNOWN;

        case ARGP_KEY_END:
            if (!globals.container_path) {
                return DRAT_ARG_ERR_NO_CONTAINER;
            }
            return ARGP_ERR_UNKNOWN;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

error_t argp_parser_command(int key, char* arg, struct argp_state* state) {
    switch (key) {
        case ARGP_KEY_ARG:
            // Record the first non-option argument as the command name
            assert(state->arg_num == 0);
            globals.command_name = arg;
            // Stop parsing args
            state->next = state->argc;
            break;
        default:
            // Do nothing; skip all global options
            break;
    }

    return 0;
}

/**
 * Argp stucture defining an Argp parser for Drat's global options.
 */
const struct argp argp_globals = {
    &argp_options_globals,
    &argp_parser_globals,
};

const struct argp argp_command = {
    &argp_options_globals,
    &argp_parser_command,
    "<command> [command option...]",

    "\n[OPTION...] refers to any number of Drat's global options, which"
    " may also be placed after <command>, and are as follows:"

    "\v<command> refers to one of Drat's commands, and [command option...]"
    " refers to any number of the options specific to that command. Run Drat"
    " without any arguments to see a list of commands, and run a Drat command"
    " without any additional arguments for info on that coammand and its options.",
};
