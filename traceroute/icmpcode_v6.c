1 #include "trace.h"
// Includes the local header file.
2 const char *
// Specifies the return type: a pointer to a constant character string.
3 icmpcode_v6(int code)
// Function to map an ICMPv6 Destination Unreachable code to a human-readable string.
4 {
5 #ifdef IPV6
// Conditional compilation block.
6 static char errbuf[100];
// Static buffer to hold the unknown error string (allows the string to persist after function returns).
7 switch (code) {
// Checks the ICMPv6 code value.
8 case ICMP6_DST_UNREACH_NOROUTE:
// Code 0: No route to destination.
9 return ("no route to host");
10 case ICMP6_DST_UNREACH_ADMIN:
// Code 1: Communication with destination administratively prohibited.
11 return ("administratively prohibited");
12 case ICMP6_DST_UNREACH_NOTNEIGHBOR:
// Code 2: Beyond scope of source address.
13 return ("not a neighbor");
14 case ICMP6_DST_UNREACH_ADDR:
// Code 3: Address unreachable.
15 return ("address unreachable");
16 case ICMP6_DST_UNREACH_NOPORT:
// Code 4: Port unreachable (handled as -1 in recv_v6, but included for completeness).
17 return ("port unreachable");
18 default:
// For any unknown or unhandled code.
19 sprintf(errbuf, "[unknown code %d]", code);
// Formats the error string into the static buffer.
20 return errbuf;
// Returns the pointer to the generic error string.
21 }
22 #endif
23 }
