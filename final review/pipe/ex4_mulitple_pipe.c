#include <stdio.h>
#include <sys/types.h>    // pid_t


void func() {
  // .....
}

int main(int argc, char *argv[]) {

  int num_procs = 8;
  // [i][0] => p -> c
  // [i][1] => p <- c
  int pipe_fd[num_procs][2][2];

  for (int i = 0; i < num_procs; i++) {

    pipe(pipe_fd[i][0]);
    pipe(pipe_fd[i][1]);

    pid_t p;

    if ((p = fork()) < 0) {}
    // child
    else if (p == 0) {
    
      // *** close all pipes that was copied from parents
      for (int prev_child = 0; prev_child < i; prev_child++) {
        close(pipe_fd[prev_child][1][0]);
      }

      // close p -> c , child write end
      close(pipe_fd[i][0][1]);
      // close p <- c, child read end
      close(pipe_fd[i][1][0]);

      // *** do something ***
      // read something from parent, write somehting to parent

      // read(pipe_fd[i][0][0], ..., ...)
      func();
      // write(pipe_fd[i][1][1], ..., ...)


      
      // done, close p -> c, child read end
      close(pipe_fd[i][0][0]);
      // done, close p <- c, child write end
      close(pipe_fd[i][1][1]);

      // *** free data ***

      exit(0);
    }
    // parent
    else {
      
      // close p -> c, parent read end
      close(pipe_fd[i][0][0]);
      
      // close p <- c, parent write end
      close(pipe_fd[i][1][1]);

      // *** do something ***
      // write(pipe_fd[i][0][1], ..., ...);

      // done, close p -> c, parent write end
      close(pipe_fd[i][0][1]);
    }
  }

  // int status;
  // while (wait(&status) > 0);

  int result;
  for (int i = 0; i < num_procs; i++) {
    read(pipe_fd[i][1][0], &result, sizeof(int));
    // done, close p <- c, parent read end
    close(pipe_fd[i][1][0]);
  }

  // *** free data ***
  return 0;
}



// 