1 #include "unp.h"
// Includes the primary header file.
2 void
// Specifies the return type (no value returned, the result is stored in *out).
3 tv_sub(struct timeval *out, struct timeval *in)
// Defines the function to subtract `*in` from `*out` (`out -= in`).
4 {
5 if ((out->tv_usec -= in->tv_usec) < 0) {
// Subtracts the microsecond fields. If the result is negative, a borrow is needed.
6 --out->tv_sec;
// Borrows one second from the seconds field.
7 out->tv_usec += 1000000;
// Adds 1,000,000 microseconds (1 second) to the microsecond field to correct for the borrow.
8 }
9 out->tv_sec -= in->tv_sec;
// Subtracts the seconds fields.
10 }
