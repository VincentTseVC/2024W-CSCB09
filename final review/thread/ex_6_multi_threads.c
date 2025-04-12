#include <stdio.h>
#include <pthread.h>


#define NUM_THREADS 8

int total_age = 0;

typedef struct thread_arg {
  int i;
  char *name;
  int age;
  long dick_size;
} pthread_arg_t;

void* func( void* arg ) {
  pthread_arg_t *person = (pthread_arg_t *) arg;

  // METHOD 1
  // grab the lock
  // total_age += person->age;
  // release the lock

  // METHOD 2
  // int *result = malloc(sizeof(int));
  // *result = person->age;
  // return result;

  // METHOD 3
  results[person->i] = person->age;
}

int results[NUM_THREADS];

int main(int argc, char *argv[]) {


  pthread_t threads[NUM_THREADS];
  pthread_arg_t args[NUM_THREADS];
  

  for (int i = 0; i < NUM_THREADS; i++) {
    // METHOD 3
    args[i].i = i;
    pthread_create(&threads[i], NULL, func, &args[i]);
  }
  
  // METHOD 1
  // for (int i = 0; i < NUM_THREADS; i++) {
  //   pthread_join(threads[i], NULL);
  // }
  // printf("%d\n", total_age);

  // METHOD 2
  // int total_age = 0;
  // int *result;
  // for (int i = 0; i < NUM_THREADS; i++) {
  //   pthread_join(threads[i], (void **) &result);
  //   total_age += *result;
  //   free(result);
  // }
  // printf("%d\n", total_age);


  // METHOD 3
  int total_age = 0;
  for (int i = 0; i < NUM_THREADS; i++) {
    total_age += results[i];
  }
  printf("%d\n", total_age);

  return 0;
}
