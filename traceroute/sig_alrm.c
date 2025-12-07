1 #include "trace.h"
// Includes the local header file.
2 int gotalarm;
// Defines the global alarm flag (declared as extern in recv functions).
3 void
4 sig_alrm(int signo)
// Defines the SIGALRM signal handler function.
5 {
6 gotalarm = 1;
// Sets the flag to 1 to signal the receive loop that the alarm has occurred.
7 return; /* and interrupt the recvfrom() */
// Returns, causing the blocking `recvfrom()` call in `recv_v4`/`recv_v6` to return EINTR.
8 }
