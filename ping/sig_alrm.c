1 #include "ping.h"
// Includes the local header file.
2 void
// Specifies the return type.
3 sig_alrm(int signo)
// Defines the signal handler function for SIGALRM.
4 {
5 (*pr->fsend) ();
// Calls the appropriate protocol-dependent sending function (send_v4 or send_v6).
6 alarm(1);
// Resets the timer to send the next SIGALRM signal in 1 second.
7 return;
// Returns from the signal handler.
8 }
