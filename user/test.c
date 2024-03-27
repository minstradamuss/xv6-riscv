#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ERROR(message) { \
  printf("Error: %s\n", message); \
  exit(1); \
}

void exit_error(char *message) {
  ERROR(message);
}

int main(int argc, char *argv[]) {
  if (argc <= 1)
    exit_error("not enough args");

  int fd1[2], fd2[2], cmutex;
  if (pipe(fd1) < 0 || pipe(fd2) < 0 || (cmutex = mopen()) < 0)
    exit_error("failed to setup");

  int cid = fork();
  if (cid < 0)
    exit_error("failed to fork");

  int pid = getpid();
  if (cid > 0) {
    if (close(fd1[0]) < 0 || close(fd2[1]) < 0)
      ERROR("parent failed to close fds");
    if (write(fd1[1], argv[1], strlen(argv[1])) != strlen(argv[1]))
      ERROR("parent failed to write");

    char c;
    while (1) {
      int k = read(fd2[0], &c, 1);
      if (k < 0)
        ERROR("parent failed to read");
      if (k == 0)
        break;
      if (lock(cmutex) < 0)
        ERROR("parent failed to lock mutex");
      printf("%d: received '%c'\n", pid, c);
      if (unlock(cmutex) < 0)
        ERROR("parent failed to unlock mutex");
    }

    if (close(fd2[0]) < 0 || close(cmutex) < 0)
      ERROR("parent failed to close fds");
  } else {
    if (close(fd1[1]) < 0 || close(fd2[0]) < 0)
      ERROR("child failed to close fds");

    char c;
    while (1) {
      int k = read(fd1[0], &c, 1);
      if (k < 0)
        ERROR("child failed to read");
      if (k == 0)
        break;
      if (lock(cmutex) < 0)
        ERROR("child failed to lock mutex");
      printf("%d: received '%c'\n", pid, c);
      if (unlock(cmutex) < 0)
        ERROR("child failed to unlock mutex");
      if (write(fd2[1], &c, 1) != 1)
        ERROR("child failed to write");
    }

    if (close(fd1[0]) < 0 || close(fd2[1]) < 0 || close(cmutex) < 0)
      ERROR("child failed to close fds");
  }

  exit(0);
}
