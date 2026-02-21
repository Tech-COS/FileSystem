#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "data_types.h"

static char *init_buffer(const char *file, size_t *size);

char *read_file(const char *file, size_t *size)
{
    char *buffer = init_buffer(file, size);
    int fd = buffer ? open(file, O_RDONLY) : -1;



    if (fd == -1) {
        if (buffer)
            free(buffer);
        return NULL;
    }

    if (read(fd, buffer, *size) == -1) {
        free(buffer);
        close(fd);
        return NULL;
    }

    close(fd);
    return buffer;
}

static char *init_buffer(const char *file, size_t *size)
{
    struct stat stat_buffer;

    if (stat(file, &stat_buffer))
        return NULL;


    *size = stat_buffer.st_size;
    return malloc(sizeof(char) * *size);
}

void write_to_disk(const char *file, char *buffer, size_t buf_size)
{
    int fd = buffer ? open(file, O_WRONLY) : -1;

    if (fd == -1) {
        if (buffer)
            free(buffer);
        return;
    }

    if (write(fd, buffer, buf_size) == -1) {
        free(buffer);
        close(fd);
        return;
    }

    free(buffer);
    close(fd);
    return;
}
