#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int isNumericChar(char c) {
    return (c != '\0' && (c < '0' || c > '9'));
}

int main(int argc, char *argv[]) {
    char inputBuffer[32];
    char *currentChar = inputBuffer;
    char *separator = inputBuffer;
    int bytesRead;

    while (1) {
        bytesRead = read(0, currentChar++, 1);

        switch (bytesRead) {
            case 0:
                printf("error: reached the end of the current file\n");
                exit(1);
            case -1:
                printf("error: couldn't read\n");
                exit(1);
        }

        if (currentChar - inputBuffer == sizeof inputBuffer) {
            printf("Buffer overflow\n");
            exit(1);
        }

        if (*(currentChar - 1) == '\n') {
            *(currentChar - 1) = '\0';
            break;
        }
    }

    while (*separator != ' ' && separator - inputBuffer < sizeof inputBuffer) {
        ++separator;
    }

    if (separator - inputBuffer == sizeof inputBuffer) {
        printf("error: couldn't find the space\n");
        exit(2);
    }
    *separator = '\0';
    ++separator;

    if (separator - inputBuffer < 2 || currentChar - separator < 2) {
        printf("error: couldn't find the second number\n");
        exit(2);
    }

    for (char *tmp = inputBuffer; tmp < currentChar; ++tmp) {
        if (isNumericChar(*tmp)) {
            printf("error: couldn't parse the numbers\n");
            exit(2);
        }
    }

    int firstNumber = atoi(inputBuffer); int secondNumber = atoi(separator);

    printf("%d\n", firstNumber + secondNumber);
    exit(0);
}