#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Incorrect number of arguments.\nUsage: %s <path to file>\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        fprintf(stderr, "\nABORT: Could not read file `%s`.\n", argv[1]);
        return -1;
    }

    off_t file_length = lseek(fd, 0, SEEK_END);
    off_t fd_position;
    
    while (1) {
        if (file_length < 4096) {
            fd_position = lseek(fd, 0, SEEK_SET);
        } else {
            fd_position = lseek(fd, file_length - 4096, SEEK_SET);
        }
            
        off_t desired_eof_position = fd_position;
        ssize_t bytes_read = 0;
        char buf[4096];

        while ( (bytes_read = read(fd, buf, sizeof(buf)))  >  0 ) {
            for (int i = 0; i < bytes_read; i++) {
                fd_position++;
                if (buf[i] != '\0') {
                    desired_eof_position = fd_position;
                }
            }
        }

        if (bytes_read != 0) {
            fprintf(stderr, "\nABORT: Error reading file `%s`.\n", argv[1]);
            close(fd);
            return -1;
        }

        if (desired_eof_position == file_length) {
            // Truncation isn't necessary anymore; stop.
            break;
        }

        if (ftruncate(fd, desired_eof_position) == -1) {
            fprintf(stderr, "FAILED.\n");
            close(fd);
            return -1;
        }
        file_length = desired_eof_position;
    }

    printf("OK.\n");

    close(fd);
    return 0;
}
