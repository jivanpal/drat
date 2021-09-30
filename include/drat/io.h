#ifndef DRAT_IO_H
#define DRAT_IO_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/errno.h>

char* get_fopen_error_msg();
char* get_fseeko_error_msg();

int open_container();
int close_container();
int end_of_container();
int read_block(void* buffer, uintmax_t block_address);
size_t read_blocks (void* buffer, uintmax_t start_block, size_t num_blocks);

// TODO: Deprecate this function
size_t write_blocks(void* buffer, uintmax_t start_block, size_t num_blocks);

#endif // DRAT_IO_H
