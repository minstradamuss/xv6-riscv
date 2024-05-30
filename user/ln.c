#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void print_error(const char *format, const char *arg1, const char *arg2) {
  printf(format, arg1, arg2);
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    printf("Usage: ln [-h/-s] old new\n");
    exit(1);
  }
  if (!strcmp(argv[1], "-h")) {
    if (link(argv[2], argv[3]) < 0)
      print_error("link %s %s: failed\n", argv[2], argv[3]);
  }
  else if (!strcmp(argv[1], "-s")) {
    if (symlink(argv[2], argv[3]) < 0)
      print_error("symlink %s %s: failed\n", argv[2], argv[3]);
  }
  exit(0);
}