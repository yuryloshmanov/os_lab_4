#ifndef LAB4_SHARED_H
#define LAB4_SHARED_H


#include <stdlib.h>
#include <stddef.h>


#define MEMORY_NAME "/lab4_shm"
#define MEMORY_SIZE 4096
#define DATA_SIZE 256


#if DATA_SIZE > MEMORY_SIZE
#warning Segfault may occur
#endif


typedef struct line_sums {
    size_t size;
    long double data[DATA_SIZE];
} line_sums_t;


void check(int result, line_sums_t *data, int const* fd, char *message) {
    if (result) {
        fprintf(stderr, "%s", message);

        if (data != NULL) {
            munmap(data, MEMORY_SIZE);
        }

        if (fd != NULL) {
            shm_unlink(MEMORY_NAME);
            close(*fd);
        }

        exit(1);
    }
}


void sigsegv_handler() {
    fprintf(stderr, "Segmentation fault occur, try decrease DATA_SIZE macro\n");
    shm_unlink(MEMORY_NAME);
    exit(1);
}


#endif //LAB4_SHARED_H
