#include "kernel/types.h"
#include "user/user.h"

void test_dynamic_allocation(char *arr) {
    int sz = (1 << 16);
    printf("ARR: creating: %d\n", wasaccess(arr, sz));
    arr[0] = 'a';
    arr[1] = 'a';
    arr[sz / 2] = 'b';
    printf("ARR: assigning elems: %d\n", wasaccess(arr, sz));
    printf("ARR: no updates: %d\n", wasaccess(arr, sz));
}

void test_static_allocation(char *larr) {
    int lsz = (1 << 11);
    if (larr == 0) {
        printf("Can't create array on stack:(\n");
        exit(1);
    }
    printf("LARR: creating: %d\n", wasaccess(larr, lsz));
    for (int i = 0; i < lsz; i++)
        larr[i] = '?';
    printf("LARR: filling: %d\n", wasaccess(larr, lsz));
    printf("LARR: no updates: %d\n", wasaccess(larr, lsz));
}

void test_element_access(char *arr, char *larr) {
    int i = 2;
    printf("Access to arr[%d] - no operations: %d\n", i, wasaccess(arr + i, sizeof(char)));
    arr[i] = 1;
    printf("Access to arr[%d] - assigning: %d\n", i, wasaccess(arr + i, sizeof(char)));

    i = 10;
    printf("Access to larr[%d] - no operations: %d\n", i, wasaccess(larr + i, sizeof(char)));
    larr[i] = '!';
    printf("Access to larr[%d] - assigning: %d\n", i, wasaccess(larr + i, sizeof(char)));
}

int main(int argc, char **argv) {
    proctable();
    
    int sz = (1 << 16);
    char *arr = malloc(sz);

    int lsz = (1 << 11);
    char larr[lsz];

    test_dynamic_allocation(arr);
    printf("-----------------------OK-----------------------------\n");
    test_static_allocation(larr);
    printf("-----------------------OK-----------------------------\n");
    test_element_access(arr, larr);
    printf("-----------------------OK-----------------------------\n");

    exit(0);
}
