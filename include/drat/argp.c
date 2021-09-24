#include "argp.h"

#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>

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
                "%s: option `--block-size` has invalid value; must be `auto` or " HEX_STRING ".\n",
                globals.program_name
            );
            break;
        case DRAT_ARG_ERR_INVALID_VOLUME:
            fprintf(
                stderr,
                "%s: option `--volume` has invalid value; must be a non-negative integer.\n",
                globals.program_name
            );
            break;
        case DRAT_ARG_ERR_INVALID_MAX_XID:
            fprintf(
                stderr,
                "%s: option `--max-xid`" INVALID_HEX_STRING,
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
    // char* name,      int key,                    char* arg,  int flags,  char* doc
    { "container",      DRAT_ARG_KEY_CONTAINER,     "path",     0,          "APFS container" },
    { "block-size",     DRAT_ARG_KEY_BLOCK_SIZE,    "size",     0,          "APFS block size in bytes" },
    { "volume",         DRAT_ARG_KEY_VOLUME,        "index",    0,          "APFS volume index" },
    { "volume-name",    DRAT_ARG_KEY_VOLUME_NAME,   "name",     0,          "APFS volume name" },
    { "max-xid",        DRAT_ARG_KEY_MAX_XID,       "xid",      0,          "Maximum transaction ID" },
    {0}
};

/**
 * Parse the decimal, hexadecimal (prefixed with `0x), or octal (prefixed
 * with `0`) integer encoded in `string`, and store the corresponding integral
 * value in `*var`, returning `true`. If `string` is not a valid value, don't
 * alter `*var`, and return `false.
 */
bool parse_number(int64_t* var, char* string) {
    char* endptr = NULL;
    intmax_t value = strtoimax(string, &endptr, 0);
    if (*string == '\0' || *endptr != '\0') {
        return false;
    }
    *var = value;
    return true;
}

/**
 * Portable version of `getsubopt()` that enforces POSIX behaviour. This is
 * used to avoid the BSD-style behaviour of `getsubopt()` seen on macOS, in
 * which what would be `value` on POSIX gets separated into `suboptarg` and
 * `value`.
 */
int	getsubopt_posix(char** optionp, char* const* tokens, char** valuep) {
    int result = getsubopt(optionp, tokens, valuep);
    
    #if !defined(_POSIX_C_SOURCE) || defined(_DARWIN_C_SOURCE)
    // We're on macOS: combine `suboptarg` and `value` into `value` to
    // replicate POSIX behaviour
    if (*valuep) {
        *(*valuep - 1) = '=';
    }
    *valuep = suboptarg;
    #endif

    return result;
}

/**
 * Argp parser function that handles Drat's global options.
 */
error_t argp_parser_globals(int key, char* arg, struct argp_state* state) {
    switch (key) {
        // Drat argument keys
        case DRAT_ARG_KEY_BLOCK_SIZE:
            if (strcmp(arg, "auto") == 0) {
                globals.block_size = 0;
                break;
            }
            if (!parse_number(&globals.block_size, arg)) {
                return DRAT_ARG_ERR_INVALID_BLOCK_SIZE;
            }
            break;
        case DRAT_ARG_KEY_VOLUME:
            if (!parse_number(&globals.volume, arg)) {
                return DRAT_ARG_ERR_INVALID_VOLUME;
            }
            break;
        case DRAT_ARG_KEY_MAX_XID:
            if (!parse_number(&globals.max_xid, arg)) {
                return DRAT_ARG_ERR_INVALID_MAX_XID;
            }
            break;
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
            // fall through
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
    " without any additional arguments for info on that command and its options.",
};

const struct argp_child argp_children[] = {
    // struct argp* argp,   int flags,  char* header
    { &argp_globals,        0,          "Global options" },
    {0}
};
