#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

char*
fmtname(char *path, int with_dir)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  int add = 0;
  if (with_dir) {
    memmove(buf, "./", 2);
    add = 2;
  }
  memmove(buf + add, p, strlen(p));
  memset(buf+strlen(p) + add, ' ', DIRSIZ-strlen(p) - add);
  if (add != 0) {
    buf[add + strlen(p)] = 0;
  }
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_NOFOLLOW)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path, 0), st.type, st.ino, st.size);
    break;
  case T_SYMLINK:
    char target[MAXPATH];
    readlink(path, target);
    printf("%s %s %d %d %l\n", fmtname(path, 0), target, st.type, st.ino, st.size);
    break;
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(lstat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      if (st.type != T_SYMLINK) 
        printf("%s %d %d %d\n", fmtname(buf, 0), st.type, st.ino, st.size);
      else {
        char target[MAXPATH];
        readlink(fmtname(buf, 1), target);
        target[st.size] = 0;
        printf("%s %s %d %d %l\n", fmtname(buf, 0), target, st.type, st.ino, st.size);
      }
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}