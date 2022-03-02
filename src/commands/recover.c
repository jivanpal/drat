#include <stdio.h>
#include <sys/errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sysexits.h>
#include <fcntl.h>

#include <apfs/object.h>
#include <apfs/nx.h>
#include <apfs/omap.h>
#include <apfs/fs.h>
#include <apfs/j.h>
#include <apfs/dstream.h>
#include <apfs/sibling.h>
#include <apfs/snap.h>

#include <drat/argp.h>
#include <drat/globals.h>
#include <drat/strings.h>
#include <drat/io.h>
#include <drat/print-fs-records.h>

#include <drat/func/boolean.h>
#include <drat/func/cksum.h>
#include <drat/func/btree.h>
#include <drat/func/j.h>

#include <drat/string/object.h>
#include <drat/string/nx.h>
#include <drat/string/omap.h>
#include <drat/string/btree.h>
#include <drat/string/fs.h>
#include <drat/string/j.h>

typedef struct {
    int64_t fsoid;  // Needs to be `int64_t to work with `parse_number()`
    char*   path;
    char*   output;
    bool    skip_multilinked_inodes;
} options_t;

#define DRAT_ARG_KEY_FSOID  (DRAT_GLOBAL_ARGS_LAST_KEY - 1)
#define DRAT_ARG_KEY_PATH   (DRAT_GLOBAL_ARGS_LAST_KEY - 2)
#define DRAT_ARG_KEY_OUTPUT (DRAT_GLOBAL_ARGS_LAST_KEY - 3)
#define DRAT_ARG_KEY_SKIP_MULTILINKED_INODES (DRAT_GLOBAL_ARGS_LAST_KEY - 4)

#define DRAT_ARG_ERR_INVALID_FSOID  (DRAT_GLOBAL_ARGS_LAST_ERR - 1)
#define DRAT_ARG_ERR_INVALID_PATH   (DRAT_GLOBAL_ARGS_LAST_ERR - 2)
#define DRAT_ARG_ERR_NO_VOLUME      (DRAT_GLOBAL_ARGS_LAST_ERR - 3)
#define DRAT_ARG_ERR_NO_ENTRYPOINT  (DRAT_GLOBAL_ARGS_LAST_ERR - 4)

static const struct argp_option argp_options[] = {
    // char* name,  int key,                char* arg,      int flags,  char* doc
    { "fsoid",      DRAT_ARG_KEY_FSOID,     "fsoid",        0,          "Filesystem object ID" },
    { "path",       DRAT_ARG_KEY_PATH,      "path",         0,          "File path" },
    { "output",     DRAT_ARG_KEY_OUTPUT,    "output path",  0,          "Path where the recovered file will be written" },
    { "skip-multilinked-inodes", DRAT_ARG_KEY_SKIP_MULTILINKED_INODES, 0, 0, "Recover multilinked files (files which have more than one hardlink) as empty files"},
    {0}
};

static error_t argp_parser(int key, char* arg, struct argp_state* state) {
    options_t* options = state->input;

    switch (key) {
        case DRAT_ARG_KEY_FSOID:
            if (!parse_number(&options->fsoid, arg)) {
                return DRAT_ARG_ERR_INVALID_FSOID;
            }
            break;
        case DRAT_ARG_KEY_PATH:
            if (arg[0] != '/') {
                return DRAT_ARG_ERR_INVALID_PATH;
            }
            options->path = arg;
            break;
        case DRAT_ARG_KEY_OUTPUT:
            options->output = arg;
            break;
        case DRAT_ARG_KEY_SKIP_MULTILINKED_INODES:
            options->skip_multilinked_inodes = true;
            break;
        case ARGP_KEY_END:
            if (globals.volume == -1) {
                return DRAT_ARG_ERR_NO_VOLUME;
            }
            if (options->fsoid == -1 && !options->path) {
                return DRAT_ARG_ERR_NO_ENTRYPOINT;
            }
            // fall through
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

// TODO: Perhaps factor out from all commands
static const struct argp argp = {
    &argp_options,  // struct argp_option* options
    &argp_parser,   // argp_parser_t parser
    0,              // char* args_doc
    0,              // char* doc
    &argp_children  // struct argp_child* children
};

static void print_usage(FILE* stream) {
    fprintf(
        stream,
        "Usage:\n"
        "  %1$s %2$s --container <container> --volume <volume index> --fsoid <filesystem object ID>\n"
        "  %1$s %2$s --container <container> --volume <volume index> --path <file path>\n"
        "Examples:\n"
        "  %1$s %2$s --container /dev/disk0s2 --volume 1 0xd02a4 --fsoid 0x3af2\n"
        "  %1$s %2$s --container /dev/disk0s2 --volume 1 0xd02a4 --path /Users/john/Documents/file.txt\n",
        globals.program_name,
        globals.command_name
    );
}

int cmd_recover(int argc, char** argv) {
    if (argc == 2) {
        // Command was specified with no other arguments
        print_usage(stdout);
        return 0;
    }
    
    // Set placeholder values so that the parser can identify whether the user
    // has set mandatory options or not
    globals.volume = -1;
    options_t options = {-1, NULL, "-"};

    bool usage_error = true;
    error_t parse_result = argp_parse(&argp, argc, argv, ARGP_IN_ORDER, 0, &options);
    if (!print_global_args_error(parse_result)) {
        switch (parse_result) {
            case 0:
                usage_error = false;
                break;
            case DRAT_ARG_ERR_INVALID_FSOID:
                fprintf(stderr, "%s: option `--fsoid`" INVALID_HEX_STRING, globals.program_name);
                break;
            case DRAT_ARG_ERR_INVALID_PATH:
                fprintf(stderr, "%s: option `--path` has invalid value; must be an absolute path, i.e. start with `/`.\n", globals.program_name);
                break;
            case DRAT_ARG_ERR_NO_VOLUME:
                fprintf(stderr, "%s: option `--volume` is mandatory.\n", globals.program_name);
                break;
            case DRAT_ARG_ERR_NO_ENTRYPOINT:
                fprintf(stderr, "%s: entrypoint is mandatory; use either `--fsoid` or `--path`.\n", globals.program_name);
                break;
            default:
                print_arg_parse_error();
                return EX_SOFTWARE;
        }
    }
    if (usage_error) {
        print_usage(stderr);
        return EX_USAGE;
    }

    char* target_dir    = NULL; // This is just the dirname of the output file (not the entire path), without trailing slash
    char* target_name   = NULL; // This is just the basename of the output file (not the entire path)
    char* file_path     = NULL; // Path to the file to write to
    FILE* file_stream   = stdout; // Stream to write the file data to
    FILE* info_stream   = stderr;

    if (strcmp(options.output, "-") != 0) {
        info_stream = stdout;

        char* last_slash = strrchr(options.output, '/');
        if (last_slash == NULL) {
            // `--output` is just a filename, e.g. `file.txt`
            target_dir = ".";
            target_name = options.output;
        } else if (*(last_slash + 1) == '\0') {
            // `--output` ends with `/` and thus is a directory, e.g. `./some/dir/` or `
            *last_slash = '\0';
            target_dir = options.output;
            target_name = strrchr(options.path, '/') + 1;
        } else {
            // `--output` is a path (either relative or absolute) with a directory component, e.g. `../path/to/file.txt`
            *last_slash = '\0';
            target_dir = options.output;
            target_name = last_slash + 1;
        }

        asprintf(&file_path, "%s/_com.dratapp.recover_%s", target_dir, target_name);
        if (!file_path) {
            fprintf(stderr, "ABORT: Not enough memory for `file_path`.\n");
            return EX_OSERR;
        }

        fprintf(info_stream, "Creating file `%s` ... ", file_path);
        int fd = open(file_path, O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR);
        if (fd < 0) {
            switch (errno) {
                case EACCES:
                    fprintf(stderr, "ABORT: The directory `%s` does not permit you to write to it.\n", target_dir);
                    return EX_CANTCREAT;
                case EDQUOT:
                    fprintf(stderr, "ABORT: You have exceeded your quota to write to the directory `%s`.\n", target_dir);
                    return EX_CANTCREAT;
                case EEXIST:
                    fprintf(stderr, "END: File `%s` already exists. Use `--overwrite` to forcefully overwite it (TODO: Not yet implemented).\n", file_path);
                    return 0;
                case EINTR:
                    fprintf(stderr, "ABORT: Our call to `open()` was interrupted by a signal.\n");
                    return EX_TEMPFAIL;
                case EIO:
                    fprintf(stderr, "ABORT: An I/O error occured whilst creating `%s`.\n", file_path);
                    return EX_IOERR;
                case EISDIR:
                    fprintf(stderr, "ABORT: A directory named `%s` already exists.\n", file_path);
                    return EX_CANTCREAT;
                case ELOOP:
                    fprintf(stderr, "ABORT: Too many symlinks.\n");
                    return EX_OSERR;
                default:
                    // TODO: Other error conditions exist for `open(2)`; handle them.
                    fprintf(stderr, "ABORT: Unknown error when attempting to open file descriptor for `%s`.\n", file_path);
                    return EX_IOERR;
            }
        }
        file_stream = fdopen(fd, "w");
        if (!file_stream) {
            fprintf(stderr, "ABORT: Error when attempting to convert file descriptor for `%s` to file pointer: %s\n", file_path, get_fopen_error_msg());
            return EX_IOERR;
        }
        fprintf(info_stream, "OK.\n");

        info_stream = stdout;
    }

    setbuf(stdout, NULL);

    // TODO: Perhaps handle other return values and factor out
    if (open_container__info_stream(info_stream) != 0) {
        return EX_NOINPUT;
    }
    fprintf(info_stream, "\n");
    
    fprintf(info_stream, "Finding most recent container superblock:\n");

    fprintf(info_stream, "- Reading block 0x0 ... ");
    nx_superblock_t* nxsb = malloc(globals.block_size);
    if (!nxsb) {
        fprintf(stderr, "ABORT: Not enough memory to create `nxsb`.\n");
        return EX_OSERR;
    }
    if (read_blocks(nxsb, 0, 1) != 1) {
        fprintf(stderr, "ABORT: Failed to read block 0.\n");
        return EX_IOERR;
    }
    fprintf(info_stream, "validating ... ");
    if (!is_cksum_valid(nxsb)) {
        fprintf(info_stream, "FAILED.\n");
        fprintf(info_stream, "WARNING: Block 0 did not validate.\n");
    } else {
        fprintf(info_stream, "OK.\n");
    }

    if (!is_nx_superblock(nxsb)) {
        fprintf(info_stream, "WARNING: Block 0 should be a container superblock, but has the wrong object type.\n\n");
    }
    if (nxsb->nx_magic != NX_MAGIC) {
        fprintf(info_stream, 
            "WARNING: Block 0 %s a container superblock but doesn't have the correct magic number.\n",
            is_nx_superblock(nxsb) ? "is" : "should be"
        );
    }

    fprintf(info_stream, "- Loading checkpoint descriptor area ... ");
    
    uint32_t xp_desc_blocks = nxsb->nx_xp_desc_blocks & ~(1 << 31);
    char (*xp_desc)[globals.block_size] = malloc(xp_desc_blocks * globals.block_size);
    if (!xp_desc) {
        fprintf(stderr, "ABORT: Not enough memory for %"PRIu32" blocks.\n", xp_desc_blocks);
        return EX_OSERR;
    }

    if (nxsb->nx_xp_desc_blocks >> 31) {
        // TODO: implement case when xp_desc area is not contiguous
        fprintf(info_stream, "END: Checkpoint descriptor area is a B-tree, but we haven't implemented handling of this case yet.\n\n");
        return 0;
    } else {
        size_t blocks_read = read_blocks(xp_desc, nxsb->nx_xp_desc_base, xp_desc_blocks);
        if (blocks_read != xp_desc_blocks) {
            fprintf(info_stream, "FAILED.\n");
            fprintf(stderr, "\nABORT: Failed to read all blocks in the checkpoint descriptor area; only successfully read %zu blocks.\n", blocks_read);
            return EX_IOERR;
        } else {
            fprintf(info_stream, "OK.\n");
        }
    }

    fprintf(info_stream, "- Searching checkpoint descriptor area ... ");
    
    uint32_t i_latest_nx = 0;
    xid_t xid_latest_nx = 0;

    for (uint32_t i = 0; i < xp_desc_blocks; i++) {
        if (!is_cksum_valid(xp_desc[i])) {
            fprintf(info_stream, "WARNING: Checkpoint descriptor block %"PRIu32" failed validation.\n", i);
            continue;
        }
        
        if (is_nx_superblock(xp_desc[i])) {
            if ( ((nx_superblock_t*)xp_desc[i])->nx_magic  !=  NX_MAGIC ) {
                fprintf(info_stream, "WARNING: Checkpoint descriptor block %"PRIu32" is a container superblock but doesn't have the correct magic number.\n", i);
                continue;
            }

            xid_t nxsb_xid = ((nx_superblock_t*)xp_desc[i])->nx_o.o_xid;

            if ( (nxsb_xid > xid_latest_nx) && (nxsb_xid <= (xid_t)(globals.max_xid)) ) {
                i_latest_nx = i;
                xid_latest_nx = nxsb_xid;
            }
        } else if (!is_checkpoint_map_phys(xp_desc[i])) {
            fprintf(info_stream, "WARNING: Checkpoint descriptor block %"PRIu32" is neither a container superblock nor a checkpoint map.\n", i);
            continue;
        }
    }

    if (xid_latest_nx == 0) {
        fprintf(info_stream, "END: Didn't find any container superblock with maximum XID %#"PRIx64".\n", globals.max_xid);
        return 0;
    }
    
    // Don't need block 0 anymore; overwrite `nxsb` with the latest container superblock.
    memcpy(nxsb, xp_desc[i_latest_nx], sizeof(nx_superblock_t));
    // Don't need the checkpoint descriptor anymore
    free(xp_desc);
    
    fprintf(info_stream, "found most recent container superblock at index %"PRIu32", its XID is %#"PRIx64".\n", i_latest_nx, nxsb->nx_o.o_xid);

    fprintf(info_stream, "Finding container's omap tree:\n");
    
    fprintf(info_stream, "- Container's omap has Physical OID %#"PRIx64".\n", nxsb->nx_omap_oid);

    fprintf(info_stream, "- Reading block %#"PRIx64" ... ", nxsb->nx_omap_oid);
    omap_phys_t* nx_omap = malloc(globals.block_size);
    if (read_blocks(nx_omap, nxsb->nx_omap_oid, 1) != 1) {
        fprintf(stderr, "ABORT: Not enough memory for `nx_omap`.\n");
        return EX_OSERR;
    }
    fprintf(info_stream, "validating ... ");
    fprintf(info_stream, is_cksum_valid(nx_omap) ? "OK.\n" : "FAILED.\n");
    
    if ((nx_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL) {
        fprintf(info_stream, "  - END: Container omap B-tree is not a Physical object, and thus cannot be located.\n");
        return 0;
    }
    fprintf(info_stream, "- Container's omap tree has Physical OID %#"PRIx64".\n", nx_omap->om_tree_oid);

    fprintf(info_stream, "- Reading block %#"PRIx64" ... ", nx_omap->om_tree_oid);
    btree_node_phys_t* nx_omap_btree = malloc(globals.block_size);
    if (!nx_omap_btree) {
        fprintf(stderr, "ABORT: Not enough memory for `nx_omap_btree`.\n");
        return EX_OSERR;
    }
    if (read_blocks(nx_omap_btree, nx_omap->om_tree_oid, 1) != 1) {
        fprintf(stderr, "ABORT: Failed to read block %#"PRIx64".\n", nx_omap->om_tree_oid);
        return EX_IOERR;
    }
    fprintf(info_stream, "validating ...");
    fprintf(info_stream, is_cksum_valid(nx_omap_btree) ? "OK.\n" : "FAILED.\n");

    fprintf(info_stream, "\n");

    // TODO: Handle `--volume-name` as well as `--volume` here.
    fprintf(info_stream, "Finding volume %"PRId64"'s superblock:\n", globals.volume);
    oid_t apsb_oid = nxsb->nx_fs_oid[globals.volume - 1];
    if (apsb_oid == 0) {
        fprintf(info_stream, "  - END: Volume %"PRId64" does not exist (apparent Virtual OID is 0).\n", globals.volume);
        return 0;
    }
    fprintf(info_stream, "- Volume %"PRId64" has Virtual OID %#"PRIx64" ... ", globals.volume, apsb_oid);

    // Get the block address
    omap_entry_t* fs_entry = get_btree_phys_omap_entry(nx_omap_btree, apsb_oid, nxsb->nx_o.o_xid);
    if (!fs_entry) {
        // TODO: Need better handling of this case; look at the previous transaction.
        fprintf(info_stream, "END: No objects with Virtual OID %#"PRIx64" and maximum XID %#"PRIx64" exist in `nx_omap_btree`.\n", apsb_oid, nxsb->nx_o.o_xid);
        return 0;
    }
    
    fprintf(info_stream, "maps to block %#"PRIx64" with XID %#"PRIx64".\n", fs_entry->val.ov_paddr, fs_entry->key.ok_xid);

    // Read the block
    fprintf(info_stream, "- Reading block %#"PRIx64" ... ", fs_entry->val.ov_paddr);
    apfs_superblock_t* apsb = malloc(globals.block_size);
    if (!apsb) {
        fprintf(stderr, "ABORT: Not enough memory for `apsb`.\n");
        return EX_OSERR;
    }
    if (read_blocks(apsb, fs_entry->val.ov_paddr, 1) != 1) {
        fprintf(stderr, "ABORT: Failed to read block %#"PRIx64".\n", fs_entry->val.ov_paddr);
        return EX_IOERR;
    }
    fprintf(info_stream, "validating ... ");
    fprintf(info_stream, is_cksum_valid(apsb) ? "OK.\n" : "FAILED.\n");
    fprintf(info_stream, "- Volume name: %s\n", apsb->apfs_volname);

    fprintf(info_stream, "Finding volume's omap tree:\n");

    fprintf(info_stream, "- Volume's omap has Physical OID %#"PRIx64".\n", apsb->apfs_omap_oid);

    fprintf(info_stream, "- Reading block %#"PRIx64" ... ", apsb->apfs_omap_oid);
    omap_phys_t* fs_omap = malloc(globals.block_size);
    if (!fs_omap) {
        fprintf(stderr, "ABORT: Not enough memory for `fs_omap`.\n");
        return EX_OSERR;
    }
    if (read_blocks(fs_omap, apsb->apfs_omap_oid, 1) != 1) {
        fprintf(stderr, "ABORT: Failed to read block %#"PRIx64".\n", apsb->apfs_omap_oid);
        return EX_IOERR;
    }
    fprintf(info_stream, "validating ... ");
    fprintf(info_stream, is_cksum_valid(fs_omap) ? "OK.\n" : "FAILED.\n");

    if ((fs_omap->om_tree_type & OBJ_STORAGETYPE_MASK) != OBJ_PHYSICAL) {
        fprintf(info_stream, "- END: Volume's omap tree is not a Physical object, and thus cannot be located.\n");
        return 0;
    }
    fprintf(info_stream, "- Volume's omap tree has Physical OID %#"PRIx64".\n", fs_omap->om_tree_oid);

    fprintf(info_stream, "- Reading block %#"PRIx64" ... ", fs_omap->om_tree_oid);
    btree_node_phys_t* fs_omap_btree = malloc(globals.block_size);
    if (!fs_omap_btree) {
        fprintf(stderr, "ABORT: Not enough memory for `fs_omap_btree`.\n");
        return EX_OSERR;
    }
    if (read_blocks(fs_omap_btree, fs_omap->om_tree_oid, 1) != 1) {
        fprintf(stderr, "ABORT: Failed to read block %#"PRIx64".\n", fs_omap->om_tree_oid);
        return EX_IOERR;
    }
    fprintf(info_stream, "validating ... ");
    fprintf(info_stream, is_cksum_valid(fs_omap_btree) ? "OK.\n" : "FAILED.\n");

    fprintf(info_stream, "\n");

    fprintf(info_stream, "Finding volume's filesystem tree:\n");
    fprintf(info_stream, "- Filesystem tree has Virtual OID %#"PRIx64" ... ", apsb->apfs_root_tree_oid);
    
    omap_entry_t* fs_root_entry = get_btree_phys_omap_entry(fs_omap_btree, apsb->apfs_root_tree_oid, apsb->apfs_o.o_xid);    
    if (!fs_root_entry) {
        // TODO: Need better handling of this case; look at the previous transaction.
        fprintf(info_stream, "END: No objects with Virtual OID %#"PRIx64" and maximum XID %#"PRIx64" exist in `fs_omap_btree`.\n", apsb->apfs_root_tree_oid, apsb->apfs_o.o_xid);
        return 0;
    }
    fprintf(info_stream, "maps to block %#"PRIx64".\n", fs_root_entry->val.ov_paddr);

    fprintf(info_stream, "- Reading block %#"PRIx64" ... ", fs_root_entry->val.ov_paddr);
    btree_node_phys_t* fs_root_btree = malloc(globals.block_size);
    if (!fs_root_btree) {
        fprintf(stderr, "ABORT: Not enough memory for `fs_root_btree`.\n");
        return EX_OSERR;
    }
    if (read_blocks(fs_root_btree, fs_root_entry->val.ov_paddr, 1) != 1) {
        fprintf(stderr, "ABORT: Failed to read block %#"PRIx64".\n", fs_root_entry->val.ov_paddr);
        return EX_IOERR;
    }
    
    // No longer need the block address of the file-system root.
    free(fs_root_entry);

    fprintf(info_stream, "validating ... ");
    fprintf(info_stream, is_cksum_valid(fs_root_btree) ? "OK.\n" : "FAILED.\n");

    fprintf(info_stream, "\n");

    j_rec_t** fs_records = NULL;

    if (options.path) {
        fprintf(info_stream, "Navigating to path `%s` ... ", options.path);

        oid_t fsoid = ROOT_DIR_INO_NUM;
        fs_records = get_fs_records(fs_omap_btree, fs_root_btree, fsoid, globals.max_xid);
        if (!fs_records) {
            fprintf(info_stream, "END: No records found for the filesystem root, `/`.\n");
            return 0;
        }

        char* path = malloc(strlen(options.path) + 1);
        if (!path) {
            fprintf(stderr, "ABORT: Not enough memory for `path`.\n");
            return EX_OSERR;
        }
        memcpy(path, options.path, strlen(options.path) + 1);

        char* path_element;
        while ( (path_element = strsep(&path, "/")) != NULL ) {
            // If path element is empty string, skip it
            if (*path_element == '\0') {
                continue;
            }
            
            signed int matching_record_index = -1;
            for (j_rec_t** fs_rec_cursor = fs_records; *fs_rec_cursor; fs_rec_cursor++) {
                j_rec_t* fs_rec = *fs_rec_cursor;
                j_key_t* hdr = fs_rec->data;
                if ( ((hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT)  ==  APFS_TYPE_DIR_REC ) {
                    j_drec_hashed_key_t* key = fs_rec->data;   
                    if (strcmp((char*)key->name, path_element) == 0) {
                        matching_record_index = fs_rec_cursor - fs_records;
                        break;
                    }
                }
            }

            if (matching_record_index == -1) {
                // No match
                fprintf(info_stream, "END: No dentry found for path `%s`.\n", options.path);
                return 0;
            }

            // Get the file ID of the matching record's target
            j_rec_t* fs_rec = fs_records[matching_record_index];
            j_drec_val_t* val = fs_rec->data + fs_rec->key_len;

            // Get the records for the target
            fsoid = val->file_id;
            free_j_rec_array(fs_records);
            fs_records = get_fs_records(fs_omap_btree, fs_root_btree, fsoid, globals.max_xid);
        }

        options.fsoid = fsoid;
        fprintf(info_stream, "its FSOID is %#"PRIx64".\n", options.fsoid);
    }

    assert(options.fsoid != -1);

    fprintf(info_stream, "Finding records for FSOID %#"PRIx64" ... ", options.fsoid);
    if (!fs_records) {
        fs_records = get_fs_records(fs_omap_btree, fs_root_btree, options.fsoid, globals.max_xid);
    }
    fprintf(info_stream, "OK.\n");

    size_t num_records = 0;
    for (j_rec_t** cursor = fs_records; *cursor; cursor++) {
        num_records++;
    }
    fprintf(info_stream, "Filesystem object has %zu records, as follows:\n", num_records);
    print_fs_records(info_stream, fs_records);

    // Output content from all matching file extents
    char* buffer = malloc(globals.block_size);
    if (!buffer) {
        fprintf(stderr, "ABORT: Not enough memory for `buffer`.\n");
        return EX_OSERR;
    }

    // Get file size
    uint64_t file_size = 0;
    for (j_rec_t** fs_rec_cursor = fs_records; *fs_rec_cursor; fs_rec_cursor++) {
        j_rec_t* fs_rec = *fs_rec_cursor;
        j_key_t* hdr = fs_rec->data;

        if ( ((hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT)  ==  APFS_TYPE_INODE ) {
            j_inode_val_t* inode = fs_rec->data + fs_rec->key_len;

            // Check if this inode is linked to by multiple paths. If so, check whether the user wants to recover the file data.
            if (inode->nlink > 1) {
                if (options.skip_multilinked_inodes) {
                    break;
                } else {
                    char answer = '\0';
                    while (true) {
                        fprintf(info_stream, "This inode is multilinked. Do you still want to recover the file data? (y/n) ");
                        scanf("%c*\n", &answer);
                        if (answer == 'y' || answer == 'n') {
                            break;
                        }
                        fprintf(info_stream, "  Invalid input.\n");
                    }
                    if (answer == 'n') {
                        break;
                    }
                }
            }
            
            file_size = get_file_size(inode, fs_rec->val_len);
            break;
        }
    }

    if (file_size == 0) {
        if (fwrite(buffer, 0, 0, file_stream) != 0) {
            fprintf(stderr, "\n\nEncountered an error writing empty file. Exiting.\n\n");
            return -1;
        }
    } else {
        // Keep track of how many bytes we still need to output
        uint64_t bytes_remaining = file_size;

        bool found_file_extent = false;
        // Go through all the fs records
        for (j_rec_t** fs_rec_cursor = fs_records; *fs_rec_cursor; fs_rec_cursor++) {
            j_rec_t* fs_rec = *fs_rec_cursor;
            j_key_t* hdr = fs_rec->data;

            // Go through all the extent records
            if ( ((hdr->obj_id_and_type & OBJ_TYPE_MASK) >> OBJ_TYPE_SHIFT)  ==  APFS_TYPE_FILE_EXTENT ) {
                found_file_extent = true;
                j_file_extent_val_t* val = fs_rec->data + fs_rec->key_len;

                // Output the content from this particular file extent
                uint64_t block_addr = val->phys_block_num;

                uint64_t extent_len_blocks = (val->len_and_flags & J_FILE_EXTENT_LEN_MASK) / globals.block_size;
                for (uint64_t i = 0;   i < extent_len_blocks;   i++, block_addr++) {
                    if (read_blocks(buffer, block_addr, 1) != 1) {
                        fprintf(stderr, "\n\nEncountered an error reading block %#"PRIx64" (block %"PRIu64" of %"PRIu64"). Exiting.\n\n", block_addr, i+1, extent_len_blocks);
                        return -1;
                    }
                    
                    uint64_t bytes_to_write = bytes_remaining < (uint64_t)globals.block_size ? bytes_remaining : (uint64_t)globals.block_size;
                    if (fwrite(buffer, bytes_to_write, 1, file_stream) != 1) {
                        fprintf(stderr, "\n\nEncountered an error writing block %"PRIu64" of %"PRIu64". Exiting.\n\n", i+1, extent_len_blocks);
                        return -1;
                    }
                    bytes_remaining -= bytes_to_write;

                    if (bytes_remaining == 0) {
                        // We've written the whole file, exit extent loop
                        break;
                    }
                }
            }

            if (bytes_remaining == 0) {
                // We've written the whole file, exit fs record loop
                break;
            }
        }
        if (!found_file_extent) {
            fprintf(info_stream, "Could not find any file extents for the specified path.\n");
        }
    }

    // Move recovered file to its proper destination
    char* true_file_path = NULL;
    asprintf(&true_file_path, "%s/%s", target_dir, target_name);
    // TODO: Check if file at `true_file_path` already exists, else it will be overwritten by `rename()`!
    rename(file_path, true_file_path);

    // Closing statements; de-allocate all memory, close all file descriptors.
    free(true_file_path);
    free_j_rec_array(fs_records);
    free(fs_omap_btree);
    free(fs_omap);
    free(nx_omap_btree);
    free(nx_omap);
    free(nxsb);
    close_container();
    fclose(file_stream);
    free(file_path);

    return 0;
}
