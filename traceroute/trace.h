1 #include "unp.h"
// Includes the primary header file for networking functions and data types
// (assumed to contain wrappers, constants, and error handling).
2 #include <netinet/in_systm.h>
// Includes system-dependent types needed for some older networking structs.
3 #include <netinet/ip.h>
// Includes the structure definition for the IPv4 header (struct ip).
4 #include <netinet/ip_icmp.h>
// Includes the structure definition for the ICMPv4 header (struct icmp).
5 #include <netinet/udp.h>
// Includes the structure definition for the UDP header (struct udphdr).

6 #define BUFSIZE 1500
// Defines the maximum size for the receive buffer.

7 struct rec { /* of outgoing UDP data */
// Structure defining the content of the data payload sent within the UDP probe.
8 u_short rec_seq; /* sequence number */
// Sequence number of the probe packet, used to match replies.
9 u_short rec_ttl; /* TTL packet left with */
// The TTL/Hop Limit value the packet was sent with (for debugging/verification).
10 struct timeval rec_tv; /* time packet left */
// Timestamp recorded just before the packet was sent (used for RTT calculation).
11 };

12 /*globals */
// Marker for the global variable declarations.
13 char recvbuf[BUFSIZE];
// Global buffer used to hold the incoming ICMP message and its encapsulated data.
14 char sendbuf[BUFSIZE];
// Global buffer used to construct and store the outgoing UDP datagram payload.
15 int datalen; /* # bytes of data following ICMP header */
// Global variable storing the size of the UDP data payload (which contains struct rec).
16 char *host;
// Global pointer to the destination hostname or IP address string.
17 u_short sport, dport;
// Global variables for the source UDP port and the initial destination UDP port.
18 int nsent; /* add 1 for each sendto() */
// Global counter for the total number of probes sent (sequence number).
19 pid_t pid; /* our PID */
// Global variable storing the process ID (used for source port generation).
20 int probe, nprobes;
// `probe`: current probe number for the current TTL (1 to nprobes); `nprobes`: total probes per TTL (default 3).
21 int sendfd, recvfd; /* send on UDP sock, read on raw ICMP sock */
// File descriptors: `sendfd` for the UDP socket, `recvfd` for the raw ICMP socket.
22 int ttl, max_ttl;
// `ttl`: current Time-To-Live/Hop Limit; `max_ttl`: maximum value for TTL/Hop Limit (default 30).
23 int verbose;
// Global flag (0 or 1) indicating if the user specified the -v option.

24 /*function prototypes */
// Marker for function prototypes.
25 const char *icmpcode_v4(int);
// Function prototype to return a string description for an ICMPv4 unreachable code.
26 const char *icmpcode_v6(int);
// Function prototype to return a string description for an ICMPv6 unreachable code.
27 int recv_v4(int, struct timeval *);
// Function prototype for receiving and processing ICMPv4 replies.
28 int recv_v6(int, struct timeval *);
// Function prototype for receiving and processing ICMPv6 replies.
29 void sig_alrm(int);
// Function prototype for the SIGALRM signal handler.
30 void traceloop(void);
// Function prototype for the main processing loop of the traceroute program.
31 void tv_sub(struct timeval *, struct timeval *);
// Function prototype for subtracting two timeval structures (used for RTT).

32 struct proto {
// Definition of a structure to handle protocol-specific differences (IPv4 vs. IPv6).
33 const char *(*icmpcode) (int);
// Function pointer to the protocol-specific ICMP code string function (icmpcode_v4 or icmpcode_v6).
34 int (*recv)(int, struct timeval *);
// Function pointer to the protocol-specific receive function (recv_v4 or recv_v6).
35 struct sockaddr *sasend; /* sockaddr{} for send, from getaddrinfo */
// Pointer to the destination socket address structure.
36 struct sockaddr *sarecv; /* sockaddr{} for receiving */
// Pointer to the socket address structure to store the source address of the ICMP reply.
37 struct sockaddr *salast; /* last sockaddr{} for receiving */
// Pointer to the socket address structure storing the address of the last successful ICMP reply for the current TTL.
38 struct sockaddr *sabind; /* sockaddr{} for binding source port */
// Pointer to the socket address structure used to bind the source UDP port.
39 socklen_t salen; /* length of sockaddr{}s */
// Length of the socket address structures.
40 int icmpproto; /* IPPROTO_xxx value for ICMP */
// IP protocol number for ICMP (IPPROTO_ICMP or IPPROTO_ICMPV6).
41 int ttllevel; /* setsockopt() level to set TTL */
// Protocol level argument for `setsockopt` when setting TTL/Hop Limit (IPPROTO_IP or IPPROTO_IPV6).
42 int ttloptname; /* setsockopt() name to set TTL */
// Option name argument for `setsockopt` when setting TTL/Hop Limit (IP_TTL or IPV6_UNICAST_HOPS).
43 } *pr;
// Global pointer *pr (protocol pointer) which will point to either proto_v4 or proto_v6 structure.

44 #ifdef IPV6
// Conditional compilation: includes IPv6-specific headers only if IPV6 is defined.
45 #include <netinet/icmp6.h>
// Includes the structure definition for the ICMPv6 header.
46 #include <netinet/ip6.h>
// Includes the structure definition for the IPv6 header.
47 #endif
// Ends the conditional compilation block.
