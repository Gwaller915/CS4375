#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// return the number of active processes in the system
// fill in user-provided data structure with pid,state,sz,ppid,name
uint64
sys_getprocs(void)
{
  uint64 addr;  // user pointer to struct pstat

  if (argaddr(0, &addr) < 0)
    return -1;
  return(procinfo(addr));
}

struct semtab {
    struct spinlock lock;
    int count;
};

//extern struct semtab semtable;

//#include "semaphore.h" // Include the semaphore data structure

//Begins initial count and lock for semaphore
int sys_sem_init(struct sem_t *s, int *p, int validValue) {
  s->count = 0;
  s->valid = validValue;
  initlock(&semtable.lock, "semtable");
  for (int i = 0; i < NSEM; i++)
    initlock(&semtable.sem[i].lock, "sem");
    
}

//Locks before reseting the count to zero
int sem_destroy(struct sem_t *s){
  acquire(&s->lock);
  s->count = 0;
  release(&s->lock);
}

// A mix of psuedo code for sem wait
//Allows process threads to be accessed one at a time depending on the count
int sem_wait(struct sem_t *s){
   acquire(&s->lock);
   while (s->count <= 0){
     s->count++;
     sleep(&s);
     
   }
   s->count--;
   release(&s->lock); 
}

//Post will increment semaphore to signal when a new resource is available.
int sem_post(struct sem_t *sem){
  acquire(&s->lock);
  s->count++;
  
  if(s->count >0){
   s-count--;
   wakeup(&s)
  }
  
  release(&sem->lock);
}

