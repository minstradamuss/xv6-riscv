#include "msg_buf.h"
#include "defs.h"
#include "types.h"
#include "proc.h"
#include <stdarg.h>

MessageBuffer_t msg_buf;
ModesTable_t modes_table;

void init_msg_buf() {
    msg_buf.head = 0;
    msg_buf.tail = 0;
    initlock(&msg_buf.lock, "buf_lock");
}

void init_mode_table() {
    initlock(&modes_table.lock, "modes_table_lock");
    modes_table.stop_ticks = -1;
    for (int i = 0; i < MODECNT; ++i) {
        modes_table.modes_enabled[i] = 0;
    }
}

void put_byte(char byte) {
    int size = PGSIZE * BUFCNT;
    msg_buf.buf[msg_buf.tail % size] = byte;
    if (msg_buf.tail - msg_buf.head == size) {
        msg_buf.head++;
        msg_buf.head %= size;
        msg_buf.tail = msg_buf.head + size;
    }
    else {
        msg_buf.tail++;
    }
}

void pr_msg_int(int xx, int base, int sign) {
    char* digits = "0123456789";
    char buf[16];
    int i;
    uint x;

    if(sign && (sign = xx < 0))
        x = -xx;
    else
        x = xx;

    i = 0;
    do {
        buf[i++] = digits[x % base];
    } while((x /= base) != 0);

    if(sign)
        buf[i++] = '-';

    while(--i >= 0)
        put_byte(buf[i]);
}

void pr_msg_ptr(uint64 p) {
    int i;
    char* digits = "0123456789abcdef";
    put_byte('0');
    put_byte('x');
    for (i = 0; i < (sizeof(uint64) * 2); i++, p <<= 4)
        put_byte(digits[p >> (sizeof(uint64) * 8 - 4)]);
}

int pr_msg(enum log_mode type, const char* fmt, ...) {
    if (fmt == 0) {
        return -1;
    }
    // acquire(&tickslock);
    int cur_ticks = ticks;
    // release(&tickslock);

    acquire(&modes_table.lock);
    if (!modes_table.modes_enabled[(int)type]) {
        release(&modes_table.lock);
        return -2;
    }
    if (modes_table.stop_ticks < cur_ticks) {
        release(&modes_table.lock);
        return -3;
    }
    release(&modes_table.lock);

    char* s;
    va_list ap;
    int i;
    char c;
    acquire(&msg_buf.lock);
    
    put_byte('[');
    pr_msg_int(cur_ticks, 10, 1);
    put_byte(']');
    put_byte(' ');

    va_start(ap, fmt);
    for(i = 0; (c = fmt[i]) != 0; i++){
        if(c != '%'){
            put_byte(c);
            continue;
        }
        c = fmt[++i];
        if(c == 0) {
            break;
        }
        switch(c){
            case 'd':
                pr_msg_int(va_arg(ap, int), 10, 1);
                break;
            case 'x':
                pr_msg_int(va_arg(ap, int), 16, 1);
                break;
            case 'p':
                pr_msg_ptr(va_arg(ap, uint64));
                break;
            case 's':
                if((s = va_arg(ap, char*)) == 0)
                    s = "(null)";
                for(; *s; s++)
                    put_byte(*s);
                break;
            case '%':
                put_byte('%');
                break;
            default:
                // Print unknown % sequence to draw attention.
                put_byte('%');
                put_byte(c);
                break;
        }
    }
    va_end(ap);
    put_byte('\n');
    release(&msg_buf.lock);
    return 0;
}

int sys_dmesg(void) {
    uint64 buf;
    argaddr(0, &buf);
    int status = 0;
    int size = BUFCNT * PGSIZE;
    acquire(&msg_buf.lock);
    if (msg_buf.tail < size) {
        if (copyout(myproc()->pagetable, buf, &msg_buf.buf[msg_buf.head], msg_buf.tail - msg_buf.head) < 0) {
            status = -1;
        }
        release(&msg_buf.lock);
        return status;    
    }
    if (copyout(myproc()->pagetable, buf, &msg_buf.buf[msg_buf.head], size - msg_buf.head) < 0) {
        release(&msg_buf.lock);
        return -1;
    }
    if (copyout(myproc()->pagetable, buf + size -  msg_buf.head, &msg_buf.buf[0], msg_buf.tail - size) < 0) {
        status = -1;
    }
    release(&msg_buf.lock);
    return status;
}

int sys_chlog(void) {
    int mode, value;
    argint(0, &mode);
    argint(1, &value);
    if (value < 0 || value > 1) {
        return -1;
    }
    if (mode < 0 || mode >= MODECNT) {
        return -2;
    }
    acquire(&modes_table.lock);
    modes_table.modes_enabled[mode] = value;
    release(&modes_table.lock);
    return 0;
}

int sys_chtime(void) {
    int time;
    argint(0, &time);
    int cur_time = ticks;
    acquire(&modes_table.lock);
    modes_table.stop_ticks = cur_time + time;
    release(&modes_table.lock);
    return 0;
}