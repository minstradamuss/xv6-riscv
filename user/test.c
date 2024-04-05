#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define ERROR_NOT_ENOUGH_ARGS "not enough args"
#define ERROR_PIPE_FAILED "failed to pipe"
#define ERROR_OPEN_MUTEX_FAILED "failed to open mutex"
#define ERROR_FORK_FAILED "failed to fork"
#define ERROR_CLOSE_FD1_PARENT_FAILED "failed to close fd1[0] (parent)"
#define ERROR_CLOSE_FD2_PARENT_FAILED "failed to close fd2[1] (parent)"
#define ERROR_WRITE_PARENT_FAILED "failed to write (parent)"
#define ERROR_CLOSE_FD1_PARENT_AFTER_WRITE_FAILED "failed to close fd1[1] (parent)"
#define ERROR_READ_PARENT_FAILED "failed to read (parent)"
#define ERROR_LOCK_MUTEX_PARENT_FAILED "failed to lock mutex (parent)"
#define ERROR_UNLOCK_MUTEX_PARENT_FAILED "failed to unlock mutex (parent)"
#define ERROR_CLOSE_FD2_PARENT_FAILED "failed to close fd2[0] (parent)"
#define ERROR_CLOSE_MUTEX_PARENT_FAILED "failed to close mutex (parent)"
#define ERROR_CLOSE_FD1_CHILD_FAILED "failed to close fd1[1] (child)"
#define ERROR_CLOSE_FD2_CHILD_FAILED "failed to close fd2[0] (child)"
#define ERROR_READ_CHILD_FAILED "failed to read (child)"
#define ERROR_LOCK_MUTEX_CHILD_FAILED "failed to lock mutex (child)"
#define ERROR_UNLOCK_MUTEX_CHILD_FAILED "failed to unlock mutex (child)"
#define ERROR_WRITE_CHILD_FAILED "failed to write (child)"
#define ERROR_CLOSE_FD1_CHILD_FAILED "failed to close fd1[0] (child)"
#define ERROR_CLOSE_FD2_CHILD_FAILED "failed to close fd2[1] (child)"
#define ERROR_CLOSE_MUTEX_CHILD_FAILED "failed to close mutex (child)"

void exit_error(char *message) {
  printf("error: %s\n", message);
  exit(1);
}

int testMutex(int argc, char *argv[]) {
  if (argc <= 1)
    exit_error(ERROR_NOT_ENOUGH_ARGS);

  int fd1[2];
  if (pipe(fd1) < 0)
    exit_error(ERROR_PIPE_FAILED);

  int fd2[2];
  if (pipe(fd2) < 0) {
    close(fd1[0]);
    close(fd1[1]);
    exit_error(ERROR_PIPE_FAILED);
  }

  int cmutex = mopen();
  if (cmutex < 0) {
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    exit_error(ERROR_OPEN_MUTEX_FAILED);
  }

  int cid = fork();
  if (cid < 0) {
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    mclose(cmutex);
    exit_error(ERROR_FORK_FAILED);
  }

  int pid = getpid();

  if (cid > 0) {
    if (close(fd1[0]) < 0) {
      close(fd1[1]);
      close(fd2[0]);
      close(fd2[1]);
      mclose(cmutex);
      exit_error(ERROR_CLOSE_FD1_PARENT_FAILED);
    }
    if (close(fd2[1]) < 0) {
      close(fd1[1]);
      close(fd2[0]);
      mclose(cmutex);
      exit_error(ERROR_CLOSE_FD2_PARENT_FAILED);
    }

    if (write(fd1[1], argv[1], strlen(argv[1])) != strlen(argv[1])) {
      close(fd1[1]);
      close(fd2[0]);
      mclose(cmutex);
      exit_error(ERROR_WRITE_PARENT_FAILED);
    }
    if (close(fd1[1]) < 0) {
      close(fd2[0]);
      mclose(cmutex);
      exit_error(ERROR_CLOSE_FD1_PARENT_AFTER_WRITE_FAILED);
    }

    char c;
    while (1) {
      int k = read(fd2[0], &c, 1);
      if (k < 0) {
        close(fd2[0]);
        mclose(cmutex);
        exit_error(ERROR_READ_PARENT_FAILED);
      }

      if (k == 0)
        break;

      if (lock(cmutex) < 0) {
        close(fd2[0]);
        mclose(cmutex);
        exit_error(ERROR_LOCK_MUTEX_PARENT_FAILED);
      }

      printf("%d: received '%c'\n", pid, c);

      if (unlock(cmutex) < 0) {
        close(fd2[0]);
        mclose(cmutex);
        exit_error(ERROR_UNLOCK_MUTEX_PARENT_FAILED);
      }
    }

    if (close(fd2[0]) < 0) {
      mclose(cmutex);
      exit_error(ERROR_CLOSE_FD2_PARENT_FAILED);
    }
    if (close(cmutex) < 0)
      exit_error(ERROR_CLOSE_MUTEX_PARENT_FAILED);

    exit(0);
  } else {
    if (close(fd1[1]) < 0) {
      close(fd1[0]);
      close(fd2[0]);
      close(fd2[1]);
      mclose(cmutex);
      exit_error(ERROR_CLOSE_FD1_CHILD_FAILED);
    }
    if (close(fd2[0]) < 0) {
      close(fd1[0]);
      close(fd2[1]);
      mclose(cmutex);
      exit_error(ERROR_CLOSE_FD2_CHILD_FAILED);
    }

    char c;
    while (1) {
      int k = read(fd1[0], &c, 1);
      if (k < 0) {
        close(fd1[0]);
        close(fd2[1]);
        mclose(cmutex);
        exit_error(ERROR_READ_CHILD_FAILED);
      }

      if (k == 0)
        break;

      if (lock(cmutex) < 0) {
        close(fd1[0]);
        close(fd2[1]);
        mclose(cmutex);
        exit_error(ERROR_LOCK_MUTEX_CHILD_FAILED);
      }

      printf("%d: received '%c'\n", pid, c);

      if (unlock(cmutex) < 0) {
        close(fd1[0]);
        close(fd2[1]);
        mclose(cmutex);
        exit_error(ERROR_UNLOCK_MUTEX_CHILD_FAILED);
      }

      if (write(fd2[1], &c, 1) != 1) {
        close(fd1[0]);
        close(fd2[1]);
        mclose(cmutex);
        exit_error(ERROR_WRITE_CHILD_FAILED);
      }
    }

    if (close(fd1[0]) < 0) {
      close(fd2[1]);
      mclose(cmutex);
      exit_error(ERROR_CLOSE_FD1_CHILD_FAILED);
    }
    if (close(fd2[1]) < 0) {
      mclose(cmutex);
      exit_error(ERROR_CLOSE_FD2_CHILD_FAILED);
    }
    if (close(cmutex) < 0)
      exit_error(ERROR_CLOSE_MUTEX_CHILD_FAILED);

    exit(0);
  }
}

int testNoMutex(int argc, char *argv[]) {
  if (argc <= 1)
    exit_error(ERROR_NOT_ENOUGH_ARGS);

  int fd1[2];
  if (pipe(fd1) < 0)
    exit_error(ERROR_PIPE_FAILED);

  int fd2[2];
  if (pipe(fd2) < 0) {
    close(fd1[0]);
    close(fd2[0]);
    exit_error(ERROR_PIPE_FAILED);
  }

  int cid = fork();
  if (cid < 0) {
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);
    exit_error(ERROR_FORK_FAILED);
  }

  int pid = getpid();

  if (cid > 0) {
    if (close(fd1[0]) < 0) {
      close(fd1[1]);
      close(fd2[0]);
      close(fd2[1]);
      exit_error(ERROR_CLOSE_FD1_PARENT_FAILED);
    }
    if (close(fd2[1]) < 0) {
      close(fd1[1]);
      close(fd2[0]);
      exit_error(ERROR_CLOSE_FD2_PARENT_FAILED);
    }

    if (write(fd1[1], argv[1], strlen(argv[1])) != strlen(argv[1])) {
      close(fd1[1]);
      close(fd2[0]);
      exit_error(ERROR_WRITE_PARENT_FAILED);
    }
    if (close(fd1[1]) < 0) {
      close(fd2[0]);
      exit_error(ERROR_CLOSE_FD1_PARENT_AFTER_WRITE_FAILED);
    }

    char c;
    while (1) {
      int k = read(fd2[0], &c, 1);
      if (k < 0) {
        close(fd2[0]);
        exit_error(ERROR_READ_PARENT_FAILED);
      }

      if (k == 0)
        break;

      printf("%d: received '%c'\n", pid, c);
    }

    if (close(fd2[0]) < 0)
      exit_error(ERROR_CLOSE_FD2_PARENT_FAILED);

    exit(0);
  } else {
    if (close(fd1[1]) < 0) {
      close(fd1[0]);
      close(fd2[0]);
      close(fd2[1]);
      exit_error(ERROR_CLOSE_FD1_CHILD_FAILED);
    }
    if (close(fd2[0]) < 0) {
      close(fd1[0]);
      close(fd2[1]);
      exit_error(ERROR_CLOSE_FD2_CHILD_FAILED);
    }

    char c;
    while (1) {
      int k = read(fd1[0], &c, 1);
      if (k < 0) {
        close(fd1[0]);
        close(fd2[1]);
        exit_error(ERROR_READ_CHILD_FAILED);
      }

      if (k == 0)
        break;

      printf("%d: received '%c'\n", pid, c);

      if (write(fd2[1], &c, 1) != 1) {
        close(fd1[0]);
        close(fd2[1]);
        exit_error(ERROR_WRITE_CHILD_FAILED);
      }
    }

    if (close(fd1[0]) < 0) {
      close(fd2[1]);
      exit_error(ERROR_CLOSE_FD1_CHILD_FAILED);
    }
    if (close(fd2[1]) < 0)
      exit_error(ERROR_CLOSE_FD2_CHILD_FAILED);

    exit(0);
  }
}

int main(int argc, char *argv[]) {
    
    printf("Running test with mutex:\n");
    testMutex(argc, argv);

    printf("\nRunning test without mutex:\n");
    testNoMutex(argc, argv);

    return 0;
}
