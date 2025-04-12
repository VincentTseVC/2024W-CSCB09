#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// void myturn() {
void* myturn( void *arg )  {
    while (1) {
        sleep(1);
        printf("my turn!\n");
    }
    return NULL;
}

void yourturn() {
    while (1) {
        sleep(2);
        printf("your turn!\n");
    }
}

int main() {

    pthread_t newthread;
    //            (&thread, attribute, func, arg)
    pthread_create(&newthread, NULL, myturn, NULL);


    // myturn();
    yourturn();
}