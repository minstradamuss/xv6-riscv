#include <stdarg.h>
#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "cycle_queue.h"


#define NULL_FMT_ERROR "error: null fmt"

struct cycle_queue buffer;

void buffer_init(void) {
    initlock(&buffer.lock, "cycle_buffer");
    buffer.head = buffer.tail = 0;
}


void print_int_buffer(int xx, int base, int sign) {
    char buf[16];
    int i = 0;
    uint x = (sign && xx < 0) ? -xx : xx;

    do {
        buf[i++] = "0123456789abcdef"[x % base];
    } while ((x /= base) != 0);

    if (sign && xx < 0)
        buf[i++] = '-';

    while (i > 0)
        queue_add(&buffer, buf[--i]);
}

void print_ptr_buffer(uint64 x) {
    int i;
    queue_add(&buffer, '0');
    queue_add(&buffer, 'x');
    for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4) {
        queue_add(&buffer, "0123456789abcdef"[x >> (sizeof(uint64) * 8 - 4)]);
    }
}

void pr_msg(const char *fmt, ...) {

    if (fmt == 0){
        panic("error: null fmt");
    }

    queue_add(&buffer, '[');

    acquire(&tickslock);
    print_int_buffer(ticks, 10, 1);
    release(&tickslock);

    queue_add(&buffer, ']');
    queue_add(&buffer, ' ');


    va_list ap;
    va_start(ap, fmt);

    int i, c;
    char *s;
    for (i = 0; (c = fmt[i] & 0xff) != 0; i++) {
        if (c != '%') {
            queue_add(&buffer, c);
            continue;
        }

        c = fmt[++i] & 0xff;
        if (c == 0)
            break;

        switch (c) {
            case 'd':
                print_int_buffer(va_arg(ap, int), 10, 1);
                break;
            case 'x':
                print_int_buffer(va_arg(ap, int), 16, 1);
                break;
            case 'p':
                print_ptr_buffer(va_arg(ap, uint64));
                break;
            case 's':
                if ((s = va_arg(ap, char*)) == 0)
                s = "(null)";
                for (; *s; s++)
                    queue_add(&buffer, *s);
                break;
            case '%':
                queue_add(&buffer, '%');
                break;
            default:
                queue_add(&buffer, '%');
                queue_add(&buffer, c);
                break;
        }
    }

    va_end(ap);

    queue_add(&buffer, '\n');
}


uint64 sys_dmesg(void) {
    uint64 addr_buf;
    uint64 sz;

    argaddr(0, &addr_buf);
    if(addr_buf == 0) return -1;
    argaddr(1, &sz);

    acquire(&buffer.lock);
    int i = 0, j = buffer.head;

    while (i < sz - 1 && j != buffer.tail) {
        if (copyout(myproc()->pagetable, addr_buf + i, &buffer.data[j], 1) < 0) {
            release(&buffer.lock);
            return -1;
        }
        i++; j = (j + 1) % BUFFER_SIZE;
    }

    char c = 0;
    if (copyout(myproc()->pagetable, addr_buf + i, &c, 1) < 0) {
        release(&buffer.lock);
        return -2;
    }

    release(&buffer.lock);
    return 0;
}