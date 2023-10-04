//#include <string.h>
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){ // puts in arguments
    
    //struct rusage rusage;

    if (argc < 2){
        printf("Arguments < 2");
        exit(0);
    }

    int start_time = uptime(); // calls uptime to start before fork
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
        int wait_time;
        wait(&wait_time);
        
        int end_time = uptime();
        printf("Time: %d\n", end_time - start_time);
        printf("Elapsed time: %d ticks\n", end_time - start_time);
    }
   
    exit(0);
}
