#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "spinlock.h"

void seminit(void) {
  initlock(&semtable.lock, "semtable");
  for (int i = 0; i < NSEM; i++)
    initlock(&semtable.sem[i].lock, "sem");
}

int semalloc(void) {
  acquire(&semtable.lock);
  for (int i = 0; i < NSEM; i++) {
    if (!holding(&semtable.sem[i].lock)) {
            initlock(&semtable.sem[i].lock, "sem");
            release(&semtable.lock);
            return i;
    }
  }
  release(&semtable.lock);
  return -1; // No empty location
}

void semdealloc(int index) {
  if (index >= 0 && index < NSEM)
    initlock(&semtable.sem[index].lock, "sem");
}

