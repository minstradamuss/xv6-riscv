#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAX_INPUT_SIZE 32

void parseInput(char *input, int *firstNum, int *secondNum);
int findSpaceAndCheck(char *start, char *end);

int main(int argc, char *argv[]) {
    
    char inputBuffer[MAX_INPUT_SIZE];
    char *currentChar = inputBuffer;
    char *spacePointer = inputBuffer;

    gets(inputBuffer, sizeof(inputBuffer) - 1);

    // Find the newline character
    for (; currentChar < inputBuffer + sizeof(inputBuffer) - 1 && *currentChar != '\n'; ++currentChar);
    ++currentChar;
    *(currentChar - 1) = '\0';

    if (!findSpaceAndCheck(spacePointer, currentChar)) {
        printf("Incorrect format: couldn't find space\n");
        exit(2);
    }

    // Check if there is at least one digit before and after the space
    if (spacePointer - inputBuffer < 2 || currentChar - spacePointer < 2) {
        printf("Incorrect format: couldn't find one of the numbers\n");
        exit(2);
    }

    // Check if both parts of the input are numeric
    for (char *tmp = inputBuffer; tmp < currentChar; ++tmp) {
        if (isNumericChar(*tmp)) {
            printf("Incorrect format: couldn't parse two numbers\n");
            exit(2);
        }
    }

    int firstNumber, secondNumber;
    parseInput(inputBuffer, &firstNumber, &secondNumber);

    printf("%d\n", add(firstNumber, secondNumber));

    exit(0);
}

void parseInput(char *input, int *firstNum, int *secondNum) {
    char *separator = input;
    for (; *separator != '\0'; ++separator);

    *separator = '\0';
    ++separator;

    *firstNum = atoi(input);
    *secondNum = atoi(separator);
}

int isNumericChar(char c) {
    return (c != '\0' && (c < '0' || c > '9'));
}

int findSpaceAndCheck(char *start, char *end) {
    for (; *start != ' ' && start < end; ++start);
    return (start != end);
}
