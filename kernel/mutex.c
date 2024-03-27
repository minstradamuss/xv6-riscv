#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "mutex.h"
#include "stat.h"
#include "proc.h"

#define MOPEN 0
#define LOCK 1
#define UNLOCK 2
#define MCLOSE 3

void mutexinit(void) {
  initlock(&mtable.lock, "mtable");
  acquire(&mtable.lock);
  for (int i = 0; i < NMUTEX; ++i)
    initsleeplock(&mtable.mutex[i].lock, "mtable_i");
  release(&mtable.lock);
}

struct mutex* mutexalloc(void) {
  struct mutex *m;
  acquire(&mtable.lock);
  for (m = mtable.mutex; m < mtable.mutex + NMUTEX; ++m) {
    if (m->ref == 0) {
      m->ref = 1;
      m->pid = -1;
      release(&mtable.lock);
      return m;
    }
  }
  release(&mtable.lock);
  return 0;
}

int mutexlock(struct mutex *m) {
  acquiresleep(&m->lock);
  m->pid = myproc()->pid;
  return 0;
}

struct mutex* mutexdup(struct mutex *m) {
  if (m->ref < 1) {
    panic("mutexdup");
  }
  m->ref++;
  return m;
}

int mutexunlock(struct mutex *m) {
  if (m->pid != myproc()->pid) {
    return -1;
  }
  m->pid = -1;
  releasesleep(&m->lock);
  return 0;
}

int mutexclose(struct mutex *m) {
  if (m->ref < 1) {
    return -1;
  }
  --m->ref;
  if (m->pid == myproc()->pid) {
    m->pid = -1;
  }
  return 0;
}

static int mdalloc(struct mutex *m) {
  struct proc *p = myproc();
  for (int md = 0; md < NOMUTEX; md++) {
    if (!p->omutex[md]) {
      p->omutex[md] = m;
      return md;
    }
  }
  return -1;
}

static int argmd(int n, int *pmd, struct mutex **pm) {
  int md;
  struct mutex *m;
  argint(n, &md);
  if (md < 0 || md >= NOMUTEX || !(m = myproc()->omutex[md])) {
    return -1;
  }
  *pmd = pmd ? md : 0;
  *pm = pm ? m : 0;
  return 0;
}

uint64 sys_mopen(void) {
  struct mutex *m;
  int md;
  if ((m = mutexalloc()) < 0 || (md = mdalloc(m)) < 0) {
    return -1;
  }
  return md;
}

uint64 sys_mutex_operation(int op) {
  struct mutex *m;
  int md;
  if (argmd(0, &md, &m) < 0) {
    return -1;
  }
  switch(op) {
    case MOPEN:
      if ((m = mutexalloc()) < 0 || (md = mdalloc(m)) < 0) {
        return -1;
      }
      break;
    case LOCK:
      return mutexlock(m);
    case UNLOCK:
      return mutexunlock(m);
    case MCLOSE:
      myproc()->omutex[md] = 0;
      return mutexclose(m);
    default:
      return -1;
  }
  return md;
}


uint64 sys_lock(void) {
  return sys_mutex_operation(LOCK);
}

uint64 sys_unlock(void) {
  return sys_mutex_operation(UNLOCK);
}

uint64 sys_mclose(void) {
  return sys_mutex_operation(MCLOSE);
}