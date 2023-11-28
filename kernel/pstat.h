#define PROT_READ (1L << 1)
#define PROT_WRITE (1L << 2)
#define MAP_SHARED 0x01 /* Share changes */
#define MAP_PRIVATE 0x02 /* Changes are private */
#define MAP_ANONYMOUS 0x20 /* No associated file */
//#define NULL 0

struct pstat {
  int pid;     // Process ID
  enum procstate state;  // Process state
  uint64 size;     // Size of process memory (bytes)
  int ppid;        // Parent process ID
  char name[16];   // Parent command name
  int priority;
  int cputime;
  int readytime; //hw3 t2
  int age;
  //int readytime[NPROC];
};

struct rusage{		//added task3 step 1
  uint cputime;
};
