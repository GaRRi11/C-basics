1 #include "unp.h"
// Includes the primary header file for networking functions and data types
// (assumed to contain wrappers for system calls, error functions, etc.).
2 #include <netinet/in_systm.h>
// Includes system-dependent types needed for some older networking structs,
// specifically for IPv4 headers and checksum computation.
3 #include <netinet/ip.h>
// Includes the structure definition for the IPv4 header (struct ip).
4 #include <netinet/ip_icmp.h>
// Includes the structure definition for the ICMPv4 header (struct icmp).

5 #define BUFSIZE 1500
// Defines the maximum size for the send and receive buffers, often chosen
// near the maximum transmission unit (MTU) to accommodate full packets.

6 /*globals */
// Marker for the global variable declarations that follow.
7 char sendbuf[BUFSIZE];
// Global buffer used to construct and store the outgoing ICMP echo request message.
8 int datalen; /* # bytes of data following ICMP header */
// Global variable storing the size (in bytes) of the optional data area
// that follows the 8-byte ICMP header (e.g., 56 bytes).
9 char *host;
// Global pointer to the string containing the hostname or IP address
// of the target host, provided as a command-line argument.
10 int nsent; /* add 1 for each sendto() */
// Global counter for the number of ICMP echo request packets sent so far;
// used to set the ICMP sequence number field.
11 pid_t pid; /* our PID */
// Global variable storing a truncated version of the process ID (PID);
// used as the ICMP identifier to filter incoming echo replies.
12 int sockfd;
// Global file descriptor for the raw socket created for sending and receiving ICMP messages.
13 int verbose;
// Global flag (0 or 1) indicating if the user specified the -v option;
// determines whether to print all received ICMP messages (not just echo replies).

14 /*function prototypes */
// Marker for the function prototypes.
15 void init_v6(void);
// Function prototype for the IPv6-specific initialization function (sets socket options).
16 void proc_v4(char *, ssize_t, struct msghdr *, struct timeval *);
// Function prototype for processing received ICMPv4 messages.
// Arguments: buffer pointer, length, message header, receive timestamp.
17 void proc_v6(char *, ssize_t, struct msghdr *, struct timeval *);
// Function prototype for processing received ICMPv6 messages.
// Arguments: buffer pointer, length, message header, receive timestamp.
18 void send_v4(void);
// Function prototype for building and sending an ICMPv4 echo request.
19 void send_v6(void);
// Function prototype for building and sending an ICMPv6 echo request.
20 void readloop(void);
// Function prototype for the main loop that continuously reads and processes
// incoming ICMP messages on the raw socket.
21 void sig_alrm(int);
// Function prototype for the SIGALRM signal handler, which drives the packet sending.
22 void tv_sub(struct timeval *, struct timeval *);
// Function prototype for subtracting one struct timeval from another (used for RTT calculation).

23 struct proto {
// Definition of a structure to handle protocol-specific differences (IPv4 vs. IPv6).
24 void (*fproc) (char *, ssize_t, struct msghdr *, struct timeval *);
// Function pointer to the protocol-specific ICMP processing function (proc_v4 or proc_v6).
25 void (*fsend) (void);
// Function pointer to the protocol-specific ICMP sending function (send_v4 or send_v6).
26 void (*finit) (void);
// Function pointer to the protocol-specific initialization function (NULL for IPv4, init_v6 for IPv6).
27 struct sockaddr *sasend; /* sockaddr{} for send, from getaddrinfo */
// Pointer to the socket address structure containing the destination IP address (for sendto).
28 struct sockaddr *sarecv; /* sockaddr{} for receiving */
// Pointer to a socket address structure allocated to receive the source address of the reply (for recvmsg).
29 socklen_t salen; /* length of sockaddr{}s */
// Length of the socket address structures (e.g., sizeof(struct sockaddr_in) or sizeof(struct sockaddr_in6)).
30 int icmpproto; /* IPPROTO_xxx value for ICMP */
// The IP protocol number for ICMP (IPPROTO_ICMP for IPv4 or IPPROTO_ICMPV6 for IPv6).
31 } *pr;
// Global pointer *pr (protocol pointer) which will point to either proto_v4 or proto_v6 structure.

32 #ifdef IPV6
// Conditional compilation: includes IPv6-specific headers only if IPV6 is defined.
33 #include <netinet/icmp6.h>
// Includes the structure definition for the ICMPv6 header (struct icmp6_hdr).
34 #include <netinet/ip6.h>
// Includes the structure definition for the IPv6 header (struct ip6_hdr).
35 #endif
// Ends the conditional compilation block.
