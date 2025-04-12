#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

// void myturn() {
void* myturn( void *arg )  {
    int *iptr = (int *) malloc(sizeof(int));
    *iptr = 5;
    for (int i = 0; i < 8; i++) {
        sleep(1);
        printf("[%d] my turn! %d\n", i, *iptr);
        (*iptr)++;
    }
    return iptr;
}

void yourturn() {
    for (int i = 0; i < 3; i++) {
        sleep(2);
        printf("[%d] your turn!\n", i);
    }
}

int main() {

    pthread_t newthread;

    int *result;

    pthread_create(&newthread, NULL, myturn, NULL);

    yourturn();

    pthread_join(newthread, (void **) &result);

    printf("thread's done: *result=%d\n", *result);

    free(result);

}