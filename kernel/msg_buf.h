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

typedef struct ModesTable {
    struct spinlock lock;
    int stop_ticks;
    int modes_enabled[MODECNT];
} ModesTable_t;

enum log_mode {SYSCALL, TRAP, SWITCH, EXEC};

void init_msg_buf();
void init_mode_table();
int pr_msg(enum log_mode type, const char* fmt, ...);