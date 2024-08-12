#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"

typedef struct MessageBuffer {
    struct spinlock lock;
    char buf[BUFCNT * PGSIZE];
    int head;
    int tail;
} MessageBuffer_t;

void init_msg_buf();
int pr_msg(const char* fmt, ...);