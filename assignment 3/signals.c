#include <assert.h>
#include <fcntl.h>
#include <semaphore.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "signals.h"

/* Given a signal number signum, return the string and set the string length
   in the given call-by-reference parameter
     Required signals and strings:
       SIGSEGV => "SEGFAULT"
       SIGHUP  => "HANGUP"
       SIGINT  => "INTERRUPT"
       SIGFPE  => "FLOATING POINT"
       SIGALRM => "ALARM CLOCK" */
char *
sig2msg (int signum, size_t *length)
{
  assert (length != NULL);
  char *msg = NULL;
  switch(signum){
  case SIGSEGV:
    msg = "SEGFAULT";
    *length = 8;
    break;
  case SIGHUP:
    msg = "HANGUP";
    *length = 6;
    break;
  case SIGINT:
    msg = "INTERRUPT";
    *length = 9;
    break;
  case SIGFPE:
    msg = "FLOATING POINT";
    *length = 14;
    break;
  case SIGALRM:
    msg = "ALARM CLOCK";
    *length = 11;
    break;

  }
  return msg;
}

/* Convert the short signal name (XXX part of SIGXXX) to its number.
   Consult /usr/include/asm/signal.h for the numerical mappings. */
int
getsig (char *name)
{
  // TODO: Complete this function for the other signals
  if (!strncmp (name, "SEGV", 4))
    return SIGSEGV; // SIGSEGV = 11
  else if (!strncmp (name, "HUP", 3))
    return SIGHUP;
  else if (!strncmp (name, "INT", 3))
    return SIGINT;
  else if (!strncmp (name, "FPE", 3))
    return SIGFPE;
  else if (!strncmp (name, "ALRM", 4))
    return SIGALRM;
  printf ("returning 0 for %s\n", name);
  return 0;
}

// FULL REQUIREMENTS criteria below this point.

// DO NOT MODIFY. This global variable is needed to preserve the execution
// context when a signal arrives.
sigjmp_buf context;

/* Reset the specified signal, allowing it to be called again. */
void
reset_signal (int signal)
{
  // Reset a triggered signal using sigprocmask (SIG_UNBLOCK...).
  // Note that this starts with an empty sigset_t, then adds ONLY
  // the unblocked signal to it.
  // See http://pubs.opengroup.org/onlinepubs/9699919799/functions/sigprocmask.html.
  sigset_t ss;
  sigemptyset (&ss);
  sigaddset (&ss, signal);
  sigprocmask (SIG_UNBLOCK, &ss, NULL);
}

/* Prints out the signal message; make this reset the signal, then
   jumps to the context with a siglongjmp().
   See http://pubs.opengroup.org/onlinepubs/9699919799/functions/siglongjmp.html. */
void
sigjmp_handler (int signum)
{
  write (STDOUT_FILENO, "SUSPEND\n", 8);
}

pid_t
run_with_jump (char *semname)
{
  // Do NOT modify these four lines, which are necessary for the timing to
  // work properly.
  sem_unlink (semname);
  sem_t *start = sem_open (semname, O_CREAT, 0644, 0);
  if (start == NULL)
    return -1;

  // Call fork() to create the child as before. The parent's code is marked
  // below. Implement the child here. The child should override ONLY the
  // SIGTSTP signal to use the sigjmp_handler() above. After overriding the
  // signal handler, the child should do this:

  //    sem_post (start);
  //    if (sigsetjmp (context, 0))
  //      {
  //        ...
  //      }
  //    while (1) ;

  // The sem_post() tells the parent the child process has started. The
  // sigsetjmp() sets a jump point that the signal handler will use. Whenever
  // the SIGTSTP signal occurs, the signal handler will jump to this jump point
  // and perform the code in the { ... }. The second time this jump point is
  // reached, do the following:

  //    printf ("Received second TSTP\n");
  //    exit (EXIT_SUCCESS);

  pid_t child = -1;

  // Parent's code. Wait until child starts, clean up the semaphore, then
  // return the child's PID.
  sem_wait (start);
  sem_unlink (semname);
  return child;
}
