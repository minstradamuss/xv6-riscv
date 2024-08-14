#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "kernel/fs.h"
#include "user/user.h"

#define N_CASES 10
#define BUFSIZE 512

char* cases[N_CASES] = {
    "000\n",
    "111\n",
    "222\n",
    "333\n",
    "444\n",
    "555\n",
    "666\n",
    "777\n",
    "888\n",
    "999\n"
};

char* fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void ls(char *path)
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
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
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
      if (st.type == T_SYMLINK) {
        ls(buf);
      } else {
        printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
      }
    }
    break;

  case T_SYMLINK:
    char symlink_buf[MAXPATH + 1];
    int n = readlink(path, symlink_buf);
    if (n < 0) {
      printf("ls: cannot read link %s\n", fmtname(path));
    }
    else {
      symlink_buf[n] = 0;
      printf("%s %d %d %l -> %s\n", fmtname(path), st.type, st.ino, st.size, symlink_buf);
    }
    break;
  }
  close(fd);
}

int generate_file(char* path, char* content) {
    int file = open(path, O_CREATE | O_RDWR);
    if (file < 0) return -1;
    write(file, content, sizeof(content));
    close(file);
    return 0;
}

int check_link(char* link, char* content) {
    char path[BUFSIZE];
    char* defpath = "/d1/d2/d3/";
    char buf[BUFSIZE];
    int n = strlen(defpath);
    for (int i = 0; i < n; i++)
        path[i] = defpath[i];
    for (int i = 0; i < strlen(link); i++) 
        path[i + n] = link[i];
    int file = open(path, O_RDWR);
    if (file < 0) {
        if (!content)
            return 0;
        return 1;
    }
    if (!content)
        return 1;
    n = read(file, buf, BUFSIZE);
    if (n < 0)
        return 1;
    if (strcmp(buf, content) != 0) 
        return 1;
    return 0;
}

int main(int argc, char const *argv[])
{
    int n;
    n = mkdir("/d1");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = mkdir("/d1/d2");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = mkdir("/d1/d2/d3");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = mkdir("/d1/d2/d3/d4");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = mkdir("/d1/d2/d3/d4/d5");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }

    n = generate_file("/d1/d2/d3/f1", cases[0]);
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = generate_file("/d1/f2", cases[1]);
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = generate_file("/d1/d2/d3/d4/d5/f3", cases[2]);
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }

    n = symlink("/d1/d2/d3/f1", "/d1/d2/d3/l1");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("./f1", "/d1/d2/d3/l2");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("./d4/d5/f3", "/d1/d2/d3/l3");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("../../f2", "/d1/d2/d3/l4");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("/d1/d2/d3/l1", "/d1/d2/d3/l5");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("/d1/d2/d3/l2", "/d1/d2/d3/l6");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("./l2", "/d1/d2/d3/l7");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("/d1/d2/d3/l8", "/d1/d2/d3/l8");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("/d1/d2/d3/l10", "/d1/d2/d3/l9");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("/d1/d2/d3/l9", "/d1/d2/d3/l10");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("/d1/d2/d3/w", "/d1/d2/d3/l11");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("./f2", "/d1/d2/d3/l12");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("./d4/d5/f1", "/d1/d2/d3/l13");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }
    n = symlink("../../f1", "/d1/d2/d3/l14");
    if (n < 0) {
        fprintf(2, "ERROR...\n");
        exit(1);
    }

    printf("=/=\n");
    ls("/");
    printf("=/d1=\n");
    ls("/d1");
    printf("=/d1/d2=\n");
    ls("/d1/d2");
    printf("=/d1/d2/d3=\n");
    ls("/d1/d2/d3");
    printf("=/d1/d2/d3/d4=\n");
    ls("/d1/d2/d3/d4");
    printf("=/d1/d2/d3/d4/d5=\n");
    ls("/d1/d2/d3/d4/d5");

    n = check_link("l1", cases[0]);
    if (n != 0) {
        fprintf(2, "1 went wrong\n");
        exit(1);
    }
    n = check_link("l2", cases[0]);
    if (n != 0) {
        fprintf(2, "2 went wrong\n");
        exit(1);
    }
    n = check_link("l3", cases[2]);
    if (n != 0) {
        fprintf(2, "3 went wrong\n");
        exit(1);
    }
    n = check_link("l4", cases[1]);
    if (n != 0) {
        fprintf(2, "4 went wrong\n");
        exit(1);
    }
    n = check_link("l5", cases[0]);
    if (n != 0) {
        fprintf(2, "5 went wrong\n");
        exit(1);
    }
    n = check_link("l6", cases[0]);
    if (n != 0) {
        fprintf(2, "6 went wrong\n");
        exit(1);
    }
    n = check_link("l7", cases[0]);
    if (n != 0) {
        fprintf(2, "7 went wrong\n");
        exit(1);
    }
    n = check_link("l8", 0);
    if (n != 0) {
        fprintf(2, "8 went wrong\n");
        exit(1);
    }
    n = check_link("l9", 0);
    if (n != 0) {
        fprintf(2, "9 went wrong\n");
        exit(1);
    }
    n = check_link("l10", 0);
    if (n != 0) {
        fprintf(2, "10 went wrong\n");
        exit(1);
    }
    n = check_link("l11", 0);
    if (n != 0) {
        fprintf(2, "11 went wrong\n");
        exit(1);
    }
    n = check_link("l12", 0);
    if (n != 0) {
        fprintf(2, "12 went wrong\n");
        exit(1);
    }
    n = check_link("l13", 0);
    if (n != 0) {
        fprintf(2, "13 went wrong\n");
        exit(1);
    }
    n = check_link("l14", 0);
    if (n != 0) {
        fprintf(2, "14 went wrong\n");
        exit(1);
    }
    printf("OK!\n");
    exit(0);
}