#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define BUFSIZE 20
#define MSG_START_PROCESS "Error: can't start new process\n"
#define MSG_WRITE_PIPE_ERROR "Error: could not write to pipe\n"

int main(int argc, char** argv) {
    int status;
    int pr[2];
    pipe(pr); 
    int pid = fork();
    if (pid == 0) {
        close(pr[1]);
        char s;
        while ((read(pr[0], &s, 1) > 0)) {
            printf("%c", s);
        }
        close(pr[0]);
        exit(0);
    }
    else if (pid < 0) {
        write(2, MSG_START_PROCESS, sizeof(MSG_START_PROCESS) - 1);
        exit(1);
    }
    else {
        close(pr[0]);
        for (int i = 1; i < argc; ++i) {
            int arg_size = strlen(argv[i]);
            if (write(pr[1], argv[i], arg_size) == -1 || write(pr[1], "\n", 1) == -1) {
                write(2, MSG_WRITE_PIPE_ERROR, sizeof(MSG_WRITE_PIPE_ERROR) - 1);
                exit(-1);
            }
        }
        close(pr[1]);
        wait(&status);
        exit(0);
    }
}