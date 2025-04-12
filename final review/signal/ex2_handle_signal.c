#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#define SECOND 5

void vc_handler(int sig) {
  // *** do anythin you want ***
  fprintf(stderr, "handling the fucking signal [%d]\n", sig);
  // exit(1);
}

int main() {

  /* adding signal handling */
  struct sigaction sa;
  sa.sa_handler = vc_handler;     // this function will run, when ever a signal recieved *anytime
  sigfillset(&sa.sa_mask);        // add ALL (31) signals to the set, we block them while vc_handler is running
  sa.sa_flags = 0;                // I dont what the fuck is this, but set it to 0, should be fine :)


  /* ONLY SIGINT and SIGQUIT are blocked while vc_handler is running
   * Other signal can still interupt the program.
   */
  // sigemptyset(&sa.sa_mask);            // clear the set
  // sigaddset(&sa.sa_mask, SIGINT);      // add to the set
  // sigaddset(&sa.sa_mask, SIGQUIT);     // add to the set

  /* set the handling action (function) to "vc_handler" for signal SIGALRM and SIGINT */
  sigaction(SIGALRM, &sa, NULL);  // When ever we recieve SIGALRM, we will run 'vc_handler'
  sigaction(SIGTSTP, &sa, NULL);   // When ever we recieve SIGSTP (Crt + Z), we will run 'vc_handler'

  while (1) {
    // do something
  }

  // SIGCHLD

  return 0;
}