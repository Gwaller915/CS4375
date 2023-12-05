#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#include "kernel/param.h"  //need for hw51a to use Prot_read

#define BSIZE 10
#define MAX 10
#define NULL 0

typedef struct {
    int buf[BSIZE];
    int nextin;
    int nextout;
    int num_produced;
    int num_consumed;
    int total;
} buffer_t;

buffer_t *buffer;

void producer()
{
   printf("L: in producer \n");
   
     while(1) {
    	printf("in while\n");
        //printf("buffer->num_produced %d", buffer->num_produced);
        if (buffer->num_produced >= MAX)
	   return;
	buffer->num_produced++;
	buffer->buf[buffer->nextin++] = buffer->num_produced;
	buffer->nextin %= BSIZE;
	
    }
    printf("L: end of producer\n");
}

void consumer()
{
  printf("Location: in consumer\n");
   while(1) {
        if (buffer->num_consumed >= MAX) 
	    return;
	buffer->total += buffer->buf[buffer->nextout++];
	buffer->nextout %= BSIZE;
	buffer->num_consumed++;
    }
  printf("Location: after while in consumer\n");
}

int
main(int argc, char *argv[])
{
    printf("In private.c\n");
    printf("___sizeof(buffer_t)%p\n", sizeof(buffer_t));

    printf("___read %p\n___write %p\n___map anon %p\n___map private %p\n", PROT_READ, PROT_WRITE,
			       MAP_ANONYMOUS, MAP_PRIVATE);

    buffer = (buffer_t *) mmap(NULL, sizeof(buffer_t),
		               PROT_READ | PROT_WRITE,
			       MAP_ANONYMOUS | MAP_PRIVATE,
			       -1, 0);
			       
			       
			       
    printf("buffer: %p\n" , buffer);		       
    buffer->nextin = 0;
    
    buffer->nextout = 0;
    buffer->num_produced = 0;
    buffer->num_consumed = 0;
    buffer->total = 0;
    

    
    producer();
    printf("L: private b4 consumer.\n");
    consumer();
    printf("L: private after consumer.\n");
    printf("total = %d\n", buffer->total);
    
    munmap(buffer, sizeof(buffer_t));  //hw4.1c
    printf("L: after munmap.\n");

    exit(0);
}

