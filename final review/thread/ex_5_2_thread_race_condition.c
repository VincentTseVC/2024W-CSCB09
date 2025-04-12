#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>


/* RUN THIS WITH:  " time ./a.out " */

#define BIG 1000000000UL

unsigned int counter = 0;

void* count_to_big( void *arg ) {
    for (unsigned int i = 0; i < BIG; i++) {
        // lock
        counter++; // c = c + 1
        // unlock
    }
    return NULL;
}

int main() {

    pthread_t t;

    pthread_create(&t, NULL, count_to_big, NULL);

    count_to_big(NULL);

    pthread_join(t, NULL);

    printf("Done. counter = %u\n", counter);
    
}

/**
 * WRONG answer and bit slower...
 * 
 *     counter++;     =>    read counter;
 *                          compute counter+1;
 *                          store new value;
 * 
 *  for example:
 * 
 *      thread #1       thread #2
 * --------------------------------------
 *      read 4  
 *                      read 4
 *      compute 4+1     compute 4+1
 *      write 5
 *                      write 5
 * 
 * 
 *  This is called "Race Condition"
 *  Two threads are racing of who gets to write first.
 * 
 *  Solution: be Atomic, when one thread is doing the increament, the other cant start...
 * 
 *  Solution in this course (B09)
 *  
 *  Use lock (mutex Lock) (mutual exclusion locks)
 *  - allow one thread to exclude other threads and say "Hey! I have the right to work in this space, everybody else has to wait."
 */