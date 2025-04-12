int main(int argc , char ** argv) {
    int n; // .....
    char *filtername; // ......
    char *filterarg[n]; // .......

    int fanout[n][2], fanin[n][2];

    // pipe creation , part (a)
    for (int i = 0; i < n; i++) {
        pipe(fanout[i]);
        pipe(fanin[n]);
    }

    int boardcastor = fork();
    if (boardcastor == 0) {
        // boardcastor code, part (b)

        // close all UN-NEEDED pipe ends
        for (int i = 0; i < n; i++) {
            close(fanout[i][0]);
            close(fanin[i][0]);
            close(fanin[i][1]);
        }

        char buff[4096];
        int num_read;
        while (1) {
            if ((num_read = read(0, buff, 4096)) <= 0) {
                // close all write-end before exit
                for (int i = 0; i < n; i++)
                    close(fanout[i][1]);
                exit(0);

                for (int i = 0; i < n; i++)
                    write(fanout[i][1], buff, num_read);
            }
        }
    }

    int collector = fork();
    if (collector == 0) {
        // collector code, part (c)

        // close all UN-NEEDED pipe ends
        for (int i = 0; i < n; i++) {
            close(fanin[i][1]);
            close(fanout[i][0]);
            close(fanout[i][1]);
        }

        double number, sum;
        while (1) {
            sum = 0;
            for (int i = 0; i < n; i++) {
                if (read(fanin[i][0], &number, sizeof(double)) <= 0) {
                    // close all write-end before exit
                    for (int i = 0; i < n; i++)
                        close(fanin[i][0]);
                    exit(0);
                }
                sum += number;
            }
            printf("%g\n", sum);
        }

        return 0;
    }

    int filter;
    // fork - exec filters, part (d)
    for (int i = 0; i < n; i++) {
        filter = fork();
        if (filter == 0) {
            dup2(fanout[i][0], 0);
            dup2(fanin[i][1], 1);

            // close all pipes
            for (int i = 0; i < n; i++) {
                close(fanin[i][0]);
                close(fanin[i][1]);
                close(fanout[i][0]);
                close(fanout[i][1]);
            }

            execlp(filtername, filtername, filterarg[i], (char *) NULL);
        }
    }

    // parent

    // wait for all children
    for (int i = 0; i < n+2; i++)
        wait(NULL);

    // close all pipes
    for (int i = 0; i < n; i++) {
        close(fanin[i][0]);
        close(fanin[i][1]);
        close(fanout[i][0]);
        close(fanout[i][1]);
    }

    return 0;
}