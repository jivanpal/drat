#ifndef DRAT_PRINT_FS_RECORDS_H
#define DRAT_PRINT_FS_RECORDS_H

#include <drat/func/btree.h>

char* get_file_name(j_rec_t** file_record);
void print_fs_records(j_rec_t** fs_records);

#endif // DRAT_PRINT_FS_RECORDS_H
