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
    close(fd);
    fd = open(target, O_RDONLY);
    close(fd);
}

void create_files_and_links() {
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
}

void test_symlinks() {
    char* to_one_global[] = {"/p2/p1/lns/a1", "/p2/p1/lns/aa1", "/p2/p1/lns/aa2", "/p2/p1/lns/oa1", "/p2/p1/lns/oa2"};
    char* to_one[] = {"o1", "oo1", "oo2"};
    char* incorrect[] = {"rec", "r2", "r3", "r1", "nex", "cur", "up", "dwn"};
    check("../../2", "o2", 1); 
    check("n1/n2/3", "o3", 1); 
    safe_chdir("../../..");
    for (int i = 0; i < 5; ++i) {
        check("/p2/p1/lns/1", to_one_global[i], 1);
    }
    safe_chdir("p2/p1/lns");
    for (int i = 0; i < 3; ++i) {
        check("1", to_one[i], 1);
    }
    for (int i = 0; i < 8; ++i) {
        check("", incorrect[i], 0);
    }
    printf("TESTS PASSED\n");
}

void test_ls() {
    char* catalogs[] = {"/", "/p2", "/p2/p1", "/p2/p1/lns", "/p2/p1/lns/n1", "/p2/p1/lns/n1/n2"};
    char* lss[] = {"/ls", "../ls", "../../ls", "../../../ls", "../../../../ls", "../../../../../ls"};
    for (int i = 0; i < 6; ++i) {
        int pid = fork();
        check_res(pid, "FAILED\n");
        if (pid == 0) {
            printf("%s\n", catalogs[i]);
            safe_chdir(catalogs[i]);
            char* argv[] = {0};
            exec(lss[i], argv);
            fprintf(2, "FAILED\n");
            exit(1);
        }
        else {
            int status;
            wait(&status);
        }
    }
}

int main() {
    mkdirs();
    create_files_and_links();
    test_symlinks();
    printf("--------------OK----------------\n");
    test_ls();
    printf("--------------OK----------------\n");
    exit(0);
}
