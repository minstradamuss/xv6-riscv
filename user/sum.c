#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    char buf[100];
    int n, total = 0;
    while((n = read(0, buf + total, 1)) > 0) {
        if(buf[total] == '\n') {
            buf[total] = '\0';
            break;
        }
        total += n;
        if(total >= sizeof(buf) - 1) break;
    }

    if(n < 0) {
        printf("Ошибка чтения\n");
        exit(1);
    }

    // Проверка на пустую строку или строку, состоящую только из символа новой строки
    if(total == 0) {
        printf("Ошибка: ввод пустой строки.\n");
        exit(1);
    }

    printf("|%s|\n", buf); // Тестовый вывод

    // Проверка на наличие только допустимых символов (цифры и пробелы)
    for(int i = 0; i < total; ++i) {
        if((buf[i] < '0' || buf[i] > '9') && buf[i] != ' ') {
            printf("Ошибка: некорректный символ во вводе.\n");
            exit(1);
        }
    }

    int a, b;
    if(sscanf(buf, "%d %d", &a, &b) == 2) {
        printf("Сумма: %d\n", a + b);
    } else {
        printf("Ошибка: некорректный формат ввода.\n");
    }

    exit(0);
}

