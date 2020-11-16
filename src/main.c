#include <sys/mman.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>


#include "shared.h"


int main(int argc, char *argv[]) {
    pid_t child_pid = fork();
    check(child_pid == -1, NULL, NULL, "Fork failed\n");

    if (child_pid == 0) {
        execl("child", "child", argc == 2 ? argv[1] : (char *)NULL, (char *)NULL);
        perror("Can't execute child\n");
        exit(1);
    } else {
        int status;
        check(waitpid(child_pid, &status, 0) == -1, NULL, NULL, "Waitpid error\n");
        if (WIFSIGNALED(status)) {
            fprintf(stderr, "Child process terminated by signal: %d\n", WTERMSIG(status));
            shm_unlink(MEMORY_NAME);
            exit(1);
        }
        if (WEXITSTATUS(status) != 0) {
            exit(1);
        }
        int fd = shm_open(MEMORY_NAME, O_RDONLY, S_IRUSR | S_IWUSR);
        check(fd == -1, NULL, &fd, "Can't open shared memory file\n");

        line_sums_t *addr = mmap(NULL, MEMORY_SIZE, PROT_READ, MAP_SHARED, fd, 0);
        check(addr == (void *)-1, addr, &fd, "Mmap error\n");

        for (int i = 0; i < addr->size; i++) {
            printf("%Lf\n", addr->data[i]);
        }

        munmap(addr, MEMORY_SIZE);
        shm_unlink(MEMORY_NAME);
        close(fd);
    }
    return 0;
}
