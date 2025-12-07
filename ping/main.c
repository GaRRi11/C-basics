1 #include "ping.h"
// Includes the local header file defined above.

2 struct proto proto_v4 =
// Defines the global structure instance for IPv4.
3 { proc_v4, send_v4, NULL, NULL, NULL, 0, IPPROTO_ICMP };
// Initializer for proto_v4: {fproc, fsend, finit, sasend, sarecv, salen, icmpproto}
// finit, sasend, sarecv, salen are NULL/0 and will be set later.
4 #ifdef IPV6
// Conditional compilation: defines the IPv6 structure only if IPV6 is enabled.
5 struct proto proto_v6 =
// Defines the global structure instance for IPv6.
6 { proc_v6, send_v6, init_v6, NULL, NULL, 0, IPPROTO_ICMPV6 };
// Initializer for proto_v6: uses proc_v6, send_v6, and init_v6.
7 #endif
// Ends the conditional compilation block.

8 int datalen = 56;
// Initializes the global variable for the data payload size to 56 bytes.
// This is the number of bytes *after* the 8-byte ICMP header.

9 int
// Specifies the return type of the main function.
10 main(int argc, char **argv)
// The main entry point of the program, receiving command line arguments.
11 {
12 int c;
// Variable to store the option character returned by getopt().
13 struct addrinfo *ai;
// Pointer to the addrinfo structure, which holds the target address information.
14 char *h;
// Temporary string pointer for the human-readable IP address string.

15 opterr = 0;
// Sets a global variable to suppress getopt() from writing error messages to stderr.
16 while ( (c = getopt(argc, argv, "v")) != -1) {
// Starts parsing command-line options; only supports "-v" (verbose).
17 switch (c) {
// Checks the option character returned by getopt.
18 case 'v':
// If the option is '-v'.
19 verbose++;
// Increments the global verbose flag.
20 break;
// Exits the switch statement.
21 case '?':
// If an unrecognized option is encountered.
22 err_quit("unrecognized option: %c", c);
// Prints an error message and terminates the program.
23 }
24 }
// Ends the while loop for option parsing.

25 if (optind != argc - 1)
// Checks if exactly one argument (the hostname) remains after option processing.
26 err_quit("usage: ping [ -v ] <hostname>");
// Prints a usage message and terminates if the number of arguments is incorrect.
27 host = argv[optind];
// Stores the required hostname argument in the global 'host' variable.

28 pid = getpid() & 0xffff;
// Gets the current process ID and truncates it to 16 bits (0 to 65535)
// to fit into the ICMP ID field.
29 Signal(SIGALRM, sig_alrm);
// Establishes the signal handler `sig_alrm` for the `SIGALRM` signal.

30 ai = Host_serv(host, NULL, 0, 0);
// Calls a custom wrapper to resolve the hostname string to an IP address,
// returning a linked list of `addrinfo` structures (only the first one is used here).
31 h = Sock_ntop_host(ai->ai_addr, ai->ai_addrlen);
// Converts the IP address from binary form to a presentation (string) form.

32 printf("PING %s (%s): %d data bytes\n",
// Prints the initial PING message to the standard output.
33 ai->ai_canonname ? ai->ai_canonname : h, h, datalen);
// Prints the canonical hostname (if available, otherwise the IP string),
// the IP address string (h), and the data payload length.

34 /* initialize according to protocol */
// Comment indicating the start of protocol-specific initialization.
35 if (ai->ai_family == AF_INET) {
// Checks if the resolved address family is IPv4.
36 pr = &proto_v4;
// Sets the global protocol pointer 'pr' to the IPv4-specific structure.
37 #ifdef IPV6
// Conditional compilation for IPv6.
38 } else if (ai->ai_family == AF_INET6) {
// Checks if the resolved address family is IPv6.
39 pr = &proto_v6;
// Sets the global protocol pointer 'pr' to the IPv6-specific structure.
40 if (IN6_IS_ADDR_V4MAPPED(&(((struct sockaddr_in6 *)
// Checks if the resolved IPv6 address is an IPv4-mapped IPv6 address (e.g., ::ffff:192.0.2.1).
41 ai->ai_addr)->sin6_addr)))
// Accesses the sin6_addr field within the sockaddr_in6 structure.
42 err_quit("cannot ping IPv4-mapped IPv6 address");
// Terminates if it is IPv4-mapped, as the program is designed to use native protocol.
43 #endif
// Ends the conditional compilation block.
44 } else
// If the address family is neither IPv4 nor IPv6.
45 err_quit("unknown address family %d", ai->ai_family);
// Terminates with an error about the unknown address family.

46 pr->sasend = ai->ai_addr;
// Sets the 'sasend' pointer in the protocol structure to point to the resolved address
// (the target address for sending packets).
47 pr->sarecv = Calloc(1, ai->ai_addrlen);
// Allocates memory for the receive socket address structure ('sarecv') using Calloc (custom wrapper).
48 pr->salen = ai->ai_addrlen;
// Stores the length of the socket address structure in 'salen'.

49 readloop();
// Calls the main function that creates the raw socket and enters the infinite
// loop for receiving and processing packets.
50 exit(0);
// Exits the program successfully upon return from readloop (which is an infinite loop,
// so this line is technically unreachable unless `readloop` terminates on error).
51 }
