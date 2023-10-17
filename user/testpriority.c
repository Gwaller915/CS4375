#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
   //printf("hello world");
   
    if (argc != 2) {
        printf("Priority: %s [...]\n", argv[0]);
        exit(1);
    }

    int new_priority = atoi(argv[1]);
    //int pid = fork();
    printf("new_priority %d\n", new_priority);
    int get = getpriority();
    
    printf("Before: %d\n", get);
    
    setpriority(new_priority);
    get = getpriority();
    printf("After: %d\n", get);
  
   exit(0);
}
