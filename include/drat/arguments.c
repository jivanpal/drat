#include "arguments.h"

#include <drat/strings.h>
#include <stdio.h>

const char* argp_program_version = VERSION_STRING;
const char* argp_program_bug_address = URL_STRING;

// Set default global option values
global_options_t globals = {
    NULL,   // program_name, set in main()
    -1,     // max_xid
    0,      // block_size (0 = auto)
};

/**
 * Argp options structure defining Drat's global options.
 */
const struct argp_option argp_options_globals[] = {
    //  char* name,     int key,                char* arg,  int flags,  char* doc
    { "container",      ARGP_KEY_CONTAINER,     "path",     0,          "APFS container" },
    { "block-size",     ARGP_KEY_BLOCK_SIZE,    "size",     0,          "APFS block size in bytes" },
    { "volume",         ARGP_KEY_VOLUME,        "index",    0,          "APFS volume index" },
    { "volume-name",    ARGP_KEY_VOLUME_NAME,   "name",     0,          "APFS volume name" },
    { "max-xid",        ARGP_KEY_MAX_XID,       "xid",      0,          "Maximum transaction ID" },
    {0}
};

/**
 * Argp parser function that handles Drat's global options.
 */
error_t argp_parser_globals(int key, char* arg, struct argp_state* state) {
    switch (key) {
        case ARGP_KEY_CONTAINER:
            //
            break;
        case ARGP_KEY_BLOCK_SIZE:
            //
            break;
        case ARGP_KEY_VOLUME:
            //
            break;
        case ARGP_KEY_VOLUME_NAME:
            //
            break;
        case ARGP_KEY_MAX_XID:
            //
            break;
        case ARGP_KEY_ARG:
            // Non-option argument
            break;
        case ARGP_KEY_ARGS:
            // All remaining arguments (this case hit if
            // we returned ARGP_ERR_UNKNOWN for ARGP_KEY_ARG)
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

error_t argp_parser_command(int key, char* arg, struct argp_state* state) {
    command_info_t* command = state->input;

    switch (key) {
        case ARGP_KEY_ARG:
            // Record the first non-option argument as the command name
            command->seen = true;
            command->name = arg;
            state->next = state->argc;  // Stop parsing args
            break;
        default:
            // Do nothing; skip all valid global option arguments
            break;
    }

    return 0;
}

/**
 * Argp stucture defining an Argp parser for Drat's global options.
 */
const struct argp argp_globals = {
    &argp_options_globals,  // const struct argp_option* options
    &argp_parser_globals,   // argp_parser_t parser
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
