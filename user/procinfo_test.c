#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "kernel/param.h"
#include "user/user.h"

#define INVALID_BUFFER_ADDRESS_ERROR "Error: invalid buffer address failed\n"
#define INSUFFICIENT_BUFFER_SIZE_ERROR "Error: insufficient buffer size failed\n"
#define VALID_BUFFER_ERROR "Error: valid buffer failed\n"

#define PS_LISTINFO_RETURNED_MSG "ps_listinfo returned: %d\n\n"

void test_invalid_buffer_address() {
    int ret = procinfo((uint64)0xFFFFFFFFFFFFF000, 10);
    
    fprintf(1, PS_LISTINFO_RETURNED_MSG, ret);
    
    if (ret > 0) {
        fprintf(2, INVALID_BUFFER_ADDRESS_ERROR);
        exit(1);
    }
}

void test_insufficient_buffer_size() {
    procinfo_t plist[1];
    int ret = procinfo((uint64)plist, 1);
    
    fprintf(1, PS_LISTINFO_RETURNED_MSG, ret);
    
    if (ret <= 1) {
        fprintf(2, INSUFFICIENT_BUFFER_SIZE_ERROR);
        exit(1);
    }
}

void test_empty_buffer() {
    int ret = procinfo(0, 10);
    
    fprintf(1, PS_LISTINFO_RETURNED_MSG, ret);
    
    if (ret < 0) {
        fprintf(2, VALID_BUFFER_ERROR);
        exit(1);
    }
}

void test_valid_buffer() {
    procinfo_t plist[10];
    int ret = procinfo((uint64)plist, 10);
    
    fprintf(1, PS_LISTINFO_RETURNED_MSG, ret);
    
    if (ret < 0) {
        fprintf(2, VALID_BUFFER_ERROR);
        exit(1);
    }
}


int main(int argc, char *argv[]) {
    test_invalid_buffer_address();
    test_insufficient_buffer_size();
    test_empty_buffer();
    test_valid_buffer();
    exit(0);
}
