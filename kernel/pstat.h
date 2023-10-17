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
