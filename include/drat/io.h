#ifndef DRAT_IO_H
#define DRAT_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/errno.h>

extern char*    nx_path;
extern FILE*    nx;
extern uint32_t nx_block_size;

void report_fopen_error(void);

size_t read_blocks (void* buffer, long start_block, size_t num_blocks);
size_t write_blocks(void* buffer, long start_block, size_t num_blocks);

#endif // DRAT_IO_H
