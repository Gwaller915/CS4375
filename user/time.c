//#include <string.h>
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
//#include "kernel/pstat.h"  //task 5 to get access to rsusage

int main(int argc, char *argv[]){ // puts in arguments
    
//    struct rusage rusage;
    {
        /* data */
    };
    

    if (argc < 2){
        printf("Arguments < 2");
        exit(0);
    }

    int pid = fork();          // forks process

    if (pid < 0){ // process should not be less than 0
        printf("PID < 0");
        exit(1);
    }

    //child process
    else if (pid == 0){
    	
        exec(argv[1], argv +1);
        //printf("In child process\n");
        exit(1);
    }else{
    //     int wait_time;
    //     wait2(&wait_time, &rusage);
    //     uint64 elapsed_time = rusage.cputime;        
    //     int cpu_usage = (rusage.cputime * 100) / elapsed_time;

    //     printf("Elapsed time: %d ticks\n", rusage.cputime);
    //     printf("CPU time: %d ticks\n", rusage.cputime);
        
    //     printf("elapsed time: %d ticks, cpu time: %d ticks, %d%% CPU\n" , rusage.cputime, rusage.cputime, cpu_usage);
     }
   
    exit(0);
}