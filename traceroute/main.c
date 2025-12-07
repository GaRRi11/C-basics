1 #include "trace.h"
// Includes the local header file.

2 struct proto proto_v4 = { icmpcode_v4, recv_v4, NULL, NULL, NULL, NULL, 0,
// Defines the global structure instance for IPv4.
3 IPPROTO_ICMP, IPPROTO_IP, IP_TTL
// Initializer for the protocol constants specific to IPv4.
4 };
5 #ifdef IPV6
// Conditional compilation for IPv6 support.
6 struct proto proto_v6 = { icmpcode_v6, recv_v6, NULL, NULL, NULL, NULL, 0,
// Defines the global structure instance for IPv6.
7 IPPROTO_ICMPV6, IPPROTO_IPV6, IPV6_UNICAST_HOPS
// Initializer for the protocol constants specific to IPv6.
8 };
9 #endif

10 int datalen = sizeof(struct rec); /* defaults */
// Sets the data payload length to the size of the `struct rec` (timestamp, seq, ttl).
11 int max_ttl = 30;
// Sets the default maximum number of hops/TTL to 30.
12 int nprobes = 3;
// Sets the default number of probe packets to send per TTL to 3.
13 u_short dport = 32768 + 666;
// Sets the initial destination UDP port number (a high, arbitrary value).

14 int
// Specifies the return type of the main function.
15 main(int argc, char **argv)
// The main entry point of the program.
16 {
17 int c;
// Variable to store the option character returned by getopt().
18 struct addrinfo *ai;
// Pointer to the addrinfo structure holding the target address information.
19 char *h;
// Temporary string pointer for the human-readable IP address string.

20 opterr = 0;
// Suppresses getopt() from writing error messages.
21 while ( (c = getopt(argc, argv, "m:v")) != -1) {
// Starts parsing command-line options; supports "-m" (max TTL) and "-v" (verbose).
22 switch (c) {
23 case 'm':
// If the option is '-m'.
24 if ( (max_ttl = atoi(optarg)) <= 1)
// Converts the argument to an integer and ensures it is greater than 1.
25 err_quit("invalid -m value");
// Terminates if the max TTL value is invalid.
26 break;
27 case 'v':
// If the option is '-v'.
28 verbose++;
// Increments the global verbose flag.
29 break;
30 case '?':
// If an unrecognized option is encountered.
31 err_quit("unrecognized option: %c", c);
// Prints an error message and terminates.
32 }
33 }

34 if (optind != argc - 1)
// Checks if exactly one argument (the hostname) remains.
35 err_quit("usage: traceroute [ -m <maxttl> -v ] <hostname>");
// Prints a usage message and terminates if the number of arguments is incorrect.
36 host = argv[optind];
// Stores the required hostname argument in the global 'host' variable.
37 pid = getpid();
// Stores the current process ID (used later for source port).
38 Signal(SIGALRM, sig_alrm);
// Establishes the signal handler `sig_alrm` for `SIGALRM` (used for timeout).

39 ai = Host_serv(host, NULL, 0, 0);
// Resolves the hostname, returning a pointer to an addrinfo structure.
40 h = Sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
// Converts the resolved IP address to a presentation string.

41 printf("traceroute to %s (%s): %d hops max, %d data bytes\n",
// Prints the initial traceroute message.
42 ai->ai_canonname ? ai->ai_canonname : h, h, max_ttl, datalen);
// Prints the canonical name or IP string, IP string, max TTL, and data length.

43 /* initialize according to protocol */
// Comment indicating the start of protocol initialization.
44 if (ai->ai_family == AF_INET) {
// Checks if the resolved address family is IPv4.
45 pr = &proto_v4;
// Sets the global protocol pointer 'pr' to the IPv4 structure.
46 #ifdef IPV6
// Conditional compilation.
47 } else if (ai->ai_family == AF_INET6) {
// Checks if the resolved address family is IPv6.
48 pr = &proto_v6;
// Sets the global protocol pointer 'pr' to the IPv6 structure.
49 if (IN6_IS_ADDR_V4MAPPED
// Checks if the resolved IPv6 address is an IPv4-mapped address.
50 (&(((struct sockaddr_in6 *) ai->ai_addr)->sin6_addr)))
51 err_quit("cannot traceroute IPv4-mapped IPv6 address");
// Terminates if it's IPv4-mapped.
52 #endif
53 } else
// If the address family is unknown.
54 err_quit("unknown address family %d", ai->ai_family);
// Terminates with an error.

55 pr->sasend = ai->ai_addr; /* contains destination address */
// Sets the 'sasend' pointer to the destination address resolved by Host_serv.
56 pr->sarecv = Calloc(1, ai->ai_addrlen);
// Allocates memory for the address structure that will hold the source of the ICMP reply.
57 pr->salast = Calloc(1, ai->ai_addrlen);
// Allocates memory for the address structure that will hold the address of the last reply for the current TTL.
58 pr->sabind = Calloc(1, ai->ai_addrlen);
// Allocates memory for the address structure used for binding the source UDP port.
59 pr->salen = ai->ai_addrlen;
// Stores the length of the socket address structures.
60 traceloop();
// Calls the main traceroute function.
61 exit(0);
// Exits the program.
62 }
