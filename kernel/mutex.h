//#pragma once

struct mutex {
  struct sleeplock lock;
  int ref;
  int pid;
};

struct mtable {
  struct spinlock lock;
  struct mutex mutex[NMUTEX];
} mtable;