#include "kernel/types.h"
#include "kernel/procinfo.h"
#include "kernel/param.h"
#include "user/user.h"

#define ERROR_PS_LISTINFO "Error: ps_listinfo failed %d\n"


int main(void) {
    int size = 1;
    int cnt_proc;

    char *states[] = {
        [STATE_UNUSED]    "UNUSED  ",
        [STATE_USED]      "USED    ",
        [STATE_SLEEPING]  "SLEEPING",
        [STATE_RUNNABLE]  "RUNNABLE",
        [STATE_RUNNING]   "RUNNING ",
        [STATE_ZOMBIE]    "ZOMBIE  "
    };

    while (1) {
        procinfo_t buf[size];
        cnt_proc = procinfo((uint64)buf, size);

        if (cnt_proc < 0) {
            fprintf(2, ERROR_PS_LISTINFO, cnt_proc);
            exit(1);
        }

        if (size < cnt_proc) {
            size *= 2;
        }
        else {
            for (int i = 0; i < cnt_proc; ++i) {
                printf("%s %d %s\n", states[buf[i].state], buf[i].parent_pid, buf[i].name);
            }
            exit(0);
        }
    }
    exit(1);
}