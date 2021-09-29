#ifndef DRAT_PRINT_FS_RECORDS_H
#define DRAT_PRINT_FS_RECORDS_H

#include <stdio.h>
#include <drat/func/btree.h>

void print_fs_records(FILE* stream, j_rec_t** fs_records);

#endif // DRAT_PRINT_FS_RECORDS_H
