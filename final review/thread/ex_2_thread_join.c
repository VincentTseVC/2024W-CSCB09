#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// void myturn() {
void* myturn( void *arg)  {
    for (int i = 0; i < 8; i++) {
        sleep(1);
        printf("[%d] my turn!\n", i);
    }
    return NULL;
}

void yourturn() {
    for (int i = 0; i < 3; i++) {
        sleep(2);
        printf("[%d] your turn!\n", i);
    }
}

int main() {

    pthread_t newthread;

    pthread_create(&newthread, NULL, myturn, NULL);

    // myturn();
    yourturn();

    // wait
    // pthread_join(newthread, NULL);  // Try to remove this line :)
}