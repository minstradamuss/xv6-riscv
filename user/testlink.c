#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void check_res(int res, char* text) {
    if (res < 0) {
        fprintf(2, text);
        exit(1);
    }
}

void safe_mkdir(char* name) {
    int res = mkdir(name);
    check_res(res, "fs error\n");
}

void safe_chdir(char* name) {
    int res  = chdir(name);
    check_res(res, "fs error\n");
}

void safe_mkfile(char* name, char data) {
    int fd = open(name, O_CREATE | O_WRONLY);
    check_res(fd, "fs error\n");
    int r = write(fd, &data, 1);
    check_res(r - 1, "fs error\n");
    close(fd);
}

void safe_symlink(char* target, char* name) {
    int res = symlink(target, name);
    check_res(res, "symlink error\n");
}

void mkdirs() {
    safe_mkdir("p2");
    safe_mkdir("p2/p1");
    safe_mkdir("p2/p1/lns");
    safe_mkdir("p2/p1/lns/n1");
    safe_mkdir("p2/p1/lns/n1/n2");
}

void check(char* target, char* linked, int good) {
    int fd = open(linked, O_RDONLY);
    if (fd == -1 && good == 0) {
        return;
    }
    check_res(fd, "Cannot open good file\n");
    check_res(good - 1, "Open bad file\n");
    char result;
    int r = read(fd, &result, 1);
    check_res(r - 1, "Cannot read from symlink\n");
    close(fd);
    fd = open(target, O_RDONLY);
    check_res(fd, "fs error\n");
    char standart;
    r = read(fd, &standart, 1);
    check_res(r - 1, "fs error\n");
    close(fd);
    if (result != standart) {
        fprintf(2, "Linked to wrong file\n");
        printf("%c %c\n", standart, result);
        exit(3);
    }
}

int main() {
    mkdirs();
    char* one_path = "/p2/p1/lns/1";
    char* two_path = "/p2/2";
    char* three_path = "/p2/p1/lns/n1/n2/3";
    safe_mkfile(one_path, '1');
    safe_mkfile(two_path, '2');
    safe_mkfile(three_path, '3');
    safe_symlink(one_path, "/p2/p1/lns/a1");
    safe_symlink("/p2/p1/lns/a1", "/p2/p1/lns/aa1");
    safe_symlink("/p2/p1/lns/aa1", "/p2/p1/lns/aa2");
    safe_symlink("/p2/p1/lns/o1", "/p2/p1/lns/oa1");
    safe_symlink("/p2/p1/lns/oa1", "/p2/p1/lns/oa2");
    safe_chdir("p2/p1/lns");
    safe_symlink("./1", "o1");
    safe_symlink("o1", "oo1");
    safe_symlink("oo1", "oo2");
    safe_symlink("../../2", "o2");
    safe_symlink("n1/n2/3", "o3");
    safe_symlink("rec", "rec");
    safe_symlink("r1", "r2");
    safe_symlink("r2", "r3");
    safe_symlink("r3", "r1");
    safe_symlink("10", "nex");
    safe_symlink("2", "cur");
    safe_symlink("../../1", "up");
    safe_symlink("n1/n2/1", "dwn");
    char* to_one_global[] = {"/p2/p1/lns/a1", "/p2/p1/lns/aa1", "/p2/p1/lns/aa2", "/p2/p1/lns/oa1", "/p2/p1/lns/oa2"};
    char* to_one[] = {"o1", "oo1", "oo2"};
    char* incorrect[] = {"rec", "r2", "r3", "r1", "nex", "cur", "up", "dwn"};
    check("../../2", "o2", 1);
    printf("o2 Done\n");
    check("n1/n2/3", "o3", 1);
    printf("o3 Done\n");
    safe_chdir("../../..");
    for (int i = 0; i < 5; ++i) {
        check(one_path, to_one_global[i], 1);
        printf("%s Done\n", to_one_global[i]);
    }
    safe_chdir("p2/p1/lns");
    for (int i = 0; i < 3; ++i) {
        check("1", to_one[i], 1);
        printf("%s Done\n", to_one[i]);
    }
    for (int i = 0; i < 8; ++i) {
        check("", incorrect[i], 0);
        printf("%s Done\n", incorrect[i]);
    }
    printf("All tests passed\n");
    char* catalogs[] = {"/", "/p2", "/p2/p1", "/p2/p1/lns", "/p2/p1/lns/n1", "/p2/p1/lns/n1/n2"};
    char* lss[] = {"/ls", "../ls", "../../ls", "../../../ls", "../../../../ls", "../../../../../ls"};
    for (int i = 0; i < 6; ++i) {
        int pid = fork();
        check_res(pid, "Fork failed\n");
        if (pid == 0) {
            printf("=============\n");
            printf("%s\n", catalogs[i]);
            printf("=============\n");
            safe_chdir(catalogs[i]);
            char* argv[] = {0};
            exec(lss[i], argv);
            fprintf(2, "exec fail\n");
            exit(1);
        }
        else {
            int status;
            wait(&status);
        }
    }
    exit(0);
}