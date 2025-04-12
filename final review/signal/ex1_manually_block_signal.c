#include <stdio.h>
#include <signal.h>

int main() {

    sigset_t set; // mask

    // clear all sigals in the set (mask)
    sigemptyset(&set);

    // add Crtl + Z to the set
    sigaddset(&set, SIGTSTP);

    // other signal to the set
    sigaddset(&set, SIGABRT);

    // process the set of signals..
    //           how        mask
    sigprocmask(SIG_BLOCK,  &set,  NULL);
              // SIG_UNBLOCK

    while (1);
}
