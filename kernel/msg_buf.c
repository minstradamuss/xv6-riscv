#include "msg_buf.h"
#include "defs.h"
#include "types.h"
#include "proc.h"
#include <stdarg.h>

MessageBuffer_t msg_buf;

void init_msg_buf() {
    msg_buf.head = 0;
    msg_buf.tail = 0;
    
    initlock(&msg_buf.lock, "buf_lock");
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

void pr_msg_int(int XX, int base, int sign) {
    char* digits = "0123456789";
    char buf[16];
    int i;
    uint x;

    if(sign && (sign = XX < 0))
        x = -XX;
    else
        x = XX;

    
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
    char* digits = "0123456789";
    put_byte('0');
    put_byte('x');
    
    for (i = 0; i < (sizeof(uint64) * 2); i++, p <<= 4)
        put_byte(digits[p >> (sizeof(uint64) * 8 - 4)]);
}

int pr_msg(const char* fmt, ...) {
    
    if (fmt == 0) {
        return -1;
    }

    acquire(&tickslock);
    int cur_ticks = ticks;
    release(&tickslock);

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

    for (i = 0; (c = fmt[i]) != 0; i++){
        if (c != '%'){
            put_byte(c);
            continue;
        }
        c = fmt[++i];
        if (c == 0) {
            break;
        }
        switch (c) {
            case 'd':
                int num1 = va_arg(ap, int);
                pr_msg_int(num1, 10, 1);
                break;
            case 'x':
                int num2 = va_arg(ap, int);
                pr_msg_int(num2, 16, 1);
                break;
            case 'p':
                uint64 ptr = va_arg(ap, uint64);
                pr_msg_ptr(ptr);
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
        int len = msg_buf.tail - msg_buf.head;
        if (copyout(myproc()->pagetable, buf, &msg_buf.buf[msg_buf.head], len) < 0) {
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