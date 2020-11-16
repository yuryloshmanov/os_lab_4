#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>


#include "shared.h"


void sum_numbers(FILE *input, line_sums_t *addr) {
    long double sum = 0;
    int flag = 0;
    addr->size = 0;
    while (1) {
        wchar_t c;
        do {
            c = fgetc(input);
        } while (c == ' ' || c == '\t');

        if (c == '\n' || c == EOF) {
            if (flag) {
                if (addr->size == DATA_SIZE) {
                    fprintf(stderr, "Buffer overflow\n");
                    return;
                }

                addr->data[addr->size++] = sum;
                sum = 0;
            }
            flag = 0;
            if (c == EOF) {
                break;
            }
        } else if (c == '-' || (c >= '0' && c <= '9') || c == '.') {
            flag = 1;
            char buff[100];
            ungetc(c, input);
            fscanf(input, "%s", buff);
            sum += strtod(buff, NULL);
        } else {
            fprintf(stderr, "Unexpected symbol while parsing file\n");
            return;
        }
    }
}


int main(int argc, char *argv[]) {
    signal(SIGSEGV, sigsegv_handler);

    int fd = shm_open(MEMORY_NAME, O_EXCL | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    check(fd == -1, NULL, &fd, "Can't open shared memory file\n");
    check(ftruncate(fd, MEMORY_SIZE) == -1, NULL, &fd, "Can't extent shared memory file\n");

    line_sums_t *addr = mmap(NULL, MEMORY_SIZE, PROT_WRITE, MAP_SHARED, fd, 0);
    check(addr == (void *)-1, addr, &fd, "Mmap error\n");
    FILE *input = NULL;
    if (argc == 2) {
        input = fopen(argv[1], "r");
    } else {
        input = fopen("input.txt", "r");
    }
    check(input == NULL, addr, &fd, "Can't open file\n");
    sum_numbers(input, addr);
    fclose(input);
    munmap(addr, MEMORY_SIZE);
    close(fd);
    return 0;
}
