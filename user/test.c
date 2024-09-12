#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define STACK_SIZE 10
#define HEAP_SIZE 8000

int
main(int argc, char const *argv[])
{
	int stack[STACK_SIZE];
	int *heap = malloc(HEAP_SIZE * sizeof(int));

	printf("Test access to the stack array:\n");
	for (int i = 0; i < STACK_SIZE; i++) {
		int accessed = checkaccess((char *)&stack[i], sizeof(int));
		printf("stack[%d] %s accessed\n", i, accessed ? "was" : "wasn't");
	}

	printf("\nTest access to the heap array:\n");
	for (int i = 0; i < HEAP_SIZE; i++) {
		int accessed = checkaccess((char *)&heap[i], sizeof(int));
		if (accessed)
			printf("heap[%d] was accessed\n");
	}

	printf("\nAccessing heap array elements...\n");
	for (int i = 0; i < HEAP_SIZE; i++) 
		heap[i] = i;

	printf("\nTest access to the heap array again:\n");
	for (int i = 0; i < HEAP_SIZE; i++) {
		int accessed = checkaccess((char *)&heap[i], sizeof(int));
		if (accessed)
			printf("heap[%d] was accessed\n", i);
	}

	free(heap);

	exit(0);
}