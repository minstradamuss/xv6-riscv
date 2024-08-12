#include "kernel/types.h"
#include "kernel/param.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/riscv.h"

int main() {
    char* buf = malloc(BUFCNT * PGSIZE + 1);
    buf[BUFCNT * PGSIZE] = 0;
    if (dmesg((uint64)buf) < 0) {
        printf("Wrong address\n");
        return 1;
    }
    printf("%s", buf);
    return 0;
}