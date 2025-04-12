#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>


/* RUN THIS WITH:  " time ./a.out " */

#define BIG 1000000000UL

unsigned int counter = 0;

void count_to_big() {
    for (unsigned int i = 0; i < BIG; i++) {
        counter++;
    }
}

int main() {

    count_to_big();
    count_to_big();

    printf("Done. counter = %u\n", counter);
    
}