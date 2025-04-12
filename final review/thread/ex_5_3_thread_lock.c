#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>


/* RUN THIS WITH:  " time ./a.out " */

#define BIG 1000000000UL

unsigned int counter = 0;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* count_to_big( void *arg ) {
    for (unsigned int i = 0; i < BIG; i++) {
        // grab the lock
        pthread_mutex_lock(&lock);      // thread will WAIT here until it can get the lock...
        counter++;
        // release when done
        pthread_mutex_unlock(&lock);
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
 * HOLY SHIT!!!!!!!!!!!!!!!!!!, its now soooo fucking slow...
 * 
 * calling lock and unlock function one billion times ... TURN TAKING is slow in CPU
 */