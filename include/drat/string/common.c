#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

/**
 * Get a human-readable representation of the value set in a given enum-field,
 * based on an enum-to-string mapping.
 * 
 * mapping:
 *      An enum-to-string mapping for the enum-field being passed. That is, an
 *      array of instances of `enum_string_mapping_t`.
 * 
 * mapping_length: The length of the array `mapping`; the number of instances of
 *      `enum_string_mapping_t` within it.
 * 
 * enum_field:
 *      The enum-field for which we want to get a string representation.
 * 
 * RETURN VALUE:
 *      String representation of the value set in the enum-field, as a C-string
 *      (char*). The caller must free this pointer when it is no longer needed.
 *      If an error occurs, NULL is returned.
 */
char* get_single_enum_string(
    const enum_string_mapping_t* mapping,
    size_t mapping_length,
    uint64_t enum_field
) {
    char* result_string = NULL;

    for (size_t i = 0; i < mapping_length; i++) {
        if (enum_field == mapping[i].value) {
            if (asprintf(&result_string, "%s", mapping[i].string) == -1) {
                fprintf(stderr, "\nERROR: %s: Couldn't allocate sufficient memory for `result_string`.\n", __func__);
                return NULL;
            }
            return result_string;
        }
    }

    // No role matched; use default string.
    if (asprintf(&result_string, "Unknown value (%#"PRIx64")", enum_field) == -1) {
        fprintf(stderr, "%s: Couldn't allocate sufficient memory for `result_string` when returning default string.\n", __func__);
        return NULL;
    }

    return result_string;
}

/**
 * Get a human-readable list of the flags that are set in a given flags-field,
 * based on an enum-to-string mapping.
 * 
 * mapping:
 *      An enum-to-string mapping for the flags-field being passed. That is, an
 *      array of instances of `enum_string_mapping_t`.
 * 
 * mapping_length: The length of the array `mapping`; the number of instances of
 *      `enum_string_mapping_t` within it.
 * 
 * flags_field:
 *      The flags-field for which we want to get a string representation.
 * 
 * single_line:
 *      Boolean value. If true, generate a comma-delimited string representation
 *      (containing no newline characters); else, generate a bulleted list
 *      (using hyphen characters for the bullets, and with each list item being
 *      terminated with a newline character).
 * 
 * RETURN VALUE:
 *      String representation of the flags that are set, as a C-string (char*).
 *      The caller must free this pointer when it is no longer needed. If an
 *      error occurs, NULL is returned.
 */
char* get_flags_enum_string(
    const enum_string_mapping_t* mapping,
    size_t mapping_length,
    uint64_t flags_field,
    bool single_line
) {
    // Initialise result buffer as empty string
    size_t bufsize = 2048;  // This should be enough for anything that call us.
    char* result_string = malloc(bufsize);
    if (!result_string) {
        fprintf(stderr, "\nERROR: %s: Couldn't allocate sufficient memory to initialise buffer.\n", __func__);
        return NULL;
    }
    *result_string = '\0';

    // Generate list
    char* list_item_format_string = single_line ? "%s, " : "- %s\n";
    size_t bytes_to_rewrite = 0;    // Used if initial write attempt fails
    size_t bytes_written = 0;
    for (size_t i = 0; i < mapping_length; i++) {
        if (flags_field & mapping[i].value) {
            bytes_to_rewrite = snprintf(
                result_string + bytes_written,
                bufsize - bytes_written,
                list_item_format_string,
                mapping[i].string
            );
            bytes_written += bytes_to_rewrite;
            
            if (bytes_written > bufsize - 1) {
                // Exhausted buffer; allocate sufficient memory and try again.
                bufsize = bytes_written + 1;
                result_string = realloc(result_string, bufsize);
                if (!result_string) {
                    fprintf(stderr, "\nERROR: %s: Couldn't allocate sufficient memory to hold entire result.\n", __func__);
                    return NULL;
                }

                snprintf(
                    result_string + (bytes_written - bytes_to_rewrite),
                    bufsize - (bytes_written - bytes_to_rewrite),
                    list_item_format_string,
                    mapping[i].string
                );
            }
        }
    }

    if (bytes_written == 0) {
        // No flags are set; use default string.
        bytes_written = snprintf(result_string, bufsize, single_line ? "(no flags)" : "- No flags.\n");
    } else {
        // At least one flag is set. If we generated a single-line list,
        // trim the trailing comma and space characters.
        if (single_line) {
            bytes_written -= 2;
            result_string[bytes_written] = '\0';
        }
    }

    // Truncate buffer
    result_string = realloc(result_string, bytes_written + 1);

    return result_string;
}
