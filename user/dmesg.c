#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/msg_buf.h"

int main(int argc, char** argv) {
    char* buf = malloc(BUFCNT * PGSIZE + 1);
    buf[BUFCNT * PGSIZE] = 0;
    chlog(SWITCH, 1);
    chlog(EXEC, 1);
    chlog(TRAP, 1);
    chlog(SYSCALL, 1);
    chtime(100);

    if (argc < 2) {
        printf("No arguments :(\n");
        exit(1);
    }
    int to_child[2];
    int to_parent[2];
    int child_stat = pipe(to_child);
    int parent_stat = pipe(to_parent);
    if (child_stat < 0 || parent_stat < 0) {
        printf("Can't create pipe\n");
        exit(2);
    }
    int pid = fork();
    if (pid < 0) {
        printf("Can't create new process\n");
        exit(3);
    }
    if (pid == 0) {
        close(to_child[1]);
        close(to_parent[0]);
        char c;
        int n;
        int my_pid = getpid();
        while ((n = read(to_child[0], &c, 1)) > 0) {
            printf("%d: received %c\n", my_pid, c);
            write(to_parent[1], &c, 1);
        }
        close(to_child[0]);
        close(to_parent[1]);
        if (n < 0) {
            printf("Reading error\n");
            exit(4);
        }
        exit(0);
    }
    close(to_child[0]);
    close(to_parent[1]);
    write(to_child[1], argv[1], strlen(argv[1]));
    close(to_child[1]);
    char c;
    int n;
    int my_pid = getpid();
    while ((n = read(to_parent[0], &c, 1)) > 0) {
        printf("%d: received %c\n", my_pid, c);
    }
    close(to_parent[0]);
    if (n < 0) {
        printf("Reading error\n");
        exit(4);
    }
    int status;
    wait(&status);

    if (dmesg((uint64)buf) < 0) {
        printf("Wrong address\n");
        return 1;
    }
    printf("%s", buf);
    free(buf);
    exit(0);
}