//#include <string.h>
#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/pstat.h"  //task 5 to get access to rsusage , order of includes matter
#include "user/user.h"



int main(int argc, char *argv[]){ // puts in arguments
    
    struct rusage rusage;
 
    if (argc < 2){
        printf("Arguments < 2");
        exit(0);
    }
    //add elapsed
    int start_time = uptime();
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

        //call 
        int wait_time;
        wait2(&wait_time, &rusage);
        int end_time = uptime();
        int elapsed_time = end_time - start_time;        
        int cpu_usage = (rusage.cputime * 100) / elapsed_time;

        printf("Elapsed time: %d ticks\n", elapsed_time);
        printf("CPU time: %d ticks\n", rusage.cputime);
        
        printf("elapsed time: %d ticks, cpu time: %d ticks, %d%% CPU\n" , elapsed_time, rusage.cputime, cpu_usage);
     }
   
    exit(0);
}