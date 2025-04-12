#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// fd = open(filename);


int main(int argc, char *argv[]) {
    
    if (argc != 3) return 0;
    
    int fd_in, fd_out;

    if ((fd_in = open(argv[1], O_RDONLY | O_CLOEXEC)) == -1) {
        perror("failed to open input file :(");
        return 1;
    }
    
    if ((fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC)) == -1) {
        perror("failed to open output file :(");
        return 1;
    }

    /**
     * Opening before fork-exec allows you to: If opening fails, dont bother
     * to fork-exec at all 
     **/

    pid_t p;
    if ((p = fork()) == -1) {
        perror("failed to fork :(");
        return 1;
    } else if (p != 0) {
        // This parent is nice and waits for child to finish :)
        wait(NULL);
        return 0;
    } else {
        
        dup2(fd_in, 0);     // redirect with STDIN
        dup2(fd_out, 1);    // redirect with STDOUT

        if (execlp("cat", "cat", (char *)NULL) == -1) {
            perror("failed to exec :(");
            return 1;
        }
    }
    return 0;
}

// cat in > out

