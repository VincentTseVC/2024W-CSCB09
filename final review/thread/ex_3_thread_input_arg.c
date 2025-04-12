#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// void myturn() {
void* myturn( void *arg )  {
    int *iptr = (int *) arg;
    for (int i = 0; i < 8; i++) {
        sleep(1);
        printf("[%d] my turn! %d\n", i, *iptr);
        (*iptr)++;
    }
    return NULL;
}

void yourturn() {
    for (int i = 0; i < 3; i++) {
        sleep(1);
        printf("[%d] your turn!\n", i);
    }
}

int main() {

    pthread_t newthread;

    int v = 5;

    pthread_create(&newthread, NULL, myturn, &v);

    yourturn();

    v += 100;   // *

    pthread_join(newthread, NULL);  // Try to remove this line :)

    printf("thread's done: v=%d\n", v);
}