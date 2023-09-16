#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
	int ticks = uptime();

	if (ticks < 0){
		exit(0);
	}

	printf("up %d clock ticks", ticks);


	exit(0);
}
