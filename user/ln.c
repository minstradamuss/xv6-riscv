#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 4){
    fprintf(2, "Usage: ln [-h/-s] old new\n");
    exit(1);
  }
  if (!strcmp(argv[1], "-h")) {
    if(link(argv[2], argv[3]) < 0)
      fprintf(2, "link %s %s: failed\n", argv[2], argv[3]);
  }
  else if (!strcmp(argv[1], "-s")) {
    if (symlink(argv[2], argv[3]) < 0) 
      fprintf(2, "symlink %s %s: failed\n", argv[2], argv[3]);
  }
  exit(0);
}