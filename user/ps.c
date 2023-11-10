#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/pstat.h"
#include "user/user.h"


int min(int a, int b) {
    return (a < b) ? a : b;
}

int
main(int argc, char **argv)
{
  struct pstat uproc[NPROC];
  int nprocs;
  int i;
  char *state;

  static char *states[] = {
    [SLEEPING]  "sleeping",
    [RUNNABLE]  "runnable",
    [RUNNING]   "running ",
    [ZOMBIE]    "zombie  "
  };

  
  
  
  
  nprocs = getprocs(uproc);
  if (nprocs < 0)
    exit(-1);

 
  int ticks = uptime();   // added to tell uptime
  printf("Uptime: %d\n", ticks);
  printf("Current Priority: %d\n", uproc->priority);
  //int effective_priority = min(MAXEFFPRIORITY, uproc[i].priority + (ticks - uproc[i].readytime));


//hw3 t2 uptime and uproc.ready added to 
  printf("pid\tstate\t\tsize\tppid\tname\tpriority\tcputime\tage\n"); //added hw3 t1
  
  //printf("pid\tstate\t\tsize\tpriority\tcputime\tppid\tname\n\tpriority\treadytime"); //added hw3 t1
  
  for (i=0; i<nprocs; i++) {
    state = states[uproc[i].state];
    //int effective_priority = min(MAXEFFPRIORITY, uproc[i].priority + (ticks - uproc[i].readytime));
    if (uproc[i].state  == RUNNABLE){
      printf("%d\t%s\t%l\t%d\t%s\t%d\t\t%d\t\t%d\n", uproc[i].pid, state,
                   uproc[i].size, uproc[i].ppid, uproc[i].name, uproc[i].priority,uproc[i].cputime, uptime - uproc[i].readytime);  
    }else{

      printf("%d\t%s\t%l\t%d\t%s\t%d\t\t%d\t\t%d\n", uproc[i].pid, state,
                   uproc[i].size, uproc[i].ppid, uproc[i].name, uproc[i].priority,uproc[i].cputime, 0);
 //              uproc[i].size, uproc[i].priority, uproc[i].cputime, uproc[i].ppid, uproc[i].name);
    }
 
  }

  //hw3 task 2

  exit(0);

}

