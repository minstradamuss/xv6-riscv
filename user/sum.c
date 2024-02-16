#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    char buf[100];
    printf("Введите два числа, разделенные пробелом: ");
    
    // Изменение с gets на fgets для предотвращения переполнения буфера
    if (fgets(buf, sizeof(buf), stdin) == NULL) {
        printf("Ошибка чтения.\n");
        exit(1);
    }
   
    int len = strlen(buf);
    if (buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        printf("Введенная строка слишком длинная.\n");
        exit(1);
    }

    int a, b;
    if (sscanf(buf, "%d %d", &a, &b) == 2) {
        printf("Сумма: %d\n", a + b);
    } else {
        printf("Ошибка: некорректный ввод. Убедитесь, что вы ввели два числа, разделенные пробелом.\n");
        exit(1);
    }

    exit(0);
}