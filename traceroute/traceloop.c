1 #include "trace.h"
// Includes the local header file.
2 void
3 traceloop(void)
// Defines the main traceroute logic function.
4 {
5 int seq, code, done;
// `seq`: overall probe sequence number; `code`: return code from recv_v4/v6; `done`: flag to exit loop.
6 double rtt;
// Variable to store the calculated Round-Trip Time (RTT).
7 struct rec *rec;
// Pointer to the data payload structure within `sendbuf`.
8 struct timeval tvrecv;
// Structure to store the timestamp when the ICMP reply is received.

9 recvfd = Socket(pr->sasend->sa_family, SOCK_RAW, pr->icmpproto);
// Creates the raw socket (`recvfd`) to receive ICMP/ICMPv6 messages.
10 setuid(getuid()); /* don’t need special permissions anymore */
// Drops superuser privileges for security (raw socket created, so privileges are no longer needed).

11 #ifdef IPV6
// Conditional block for IPv6-specific setup.
12 if (pr->sasend->sa_family == AF_INET6 && verbose == 0) {
// If using IPv6 and not in verbose mode, set up an ICMPv6 filter.
13 struct icmp6_filter myfilt;
// Declares the filter structure.
14 ICMP6_FILTER_SETBLOCKALL(&myfilt);
// Blocks all ICMPv6 message types initially.
15 ICMP6_FILTER_SETPASS(ICMP6_TIME_EXCEEDED, &myfilt);
// Allows ICMPv6 Time Exceeded messages to pass (intermediate routers).
16 ICMP6_FILTER_SETPASS(ICMP6_DST_UNREACH, &myfilt);
// Allows ICMPv6 Destination Unreachable messages to pass (final destination or failure).
17 setsockopt(recvfd, IPPROTO_IPV6, ICMP6_FILTER,
// Applies the filter to the raw socket.
18 &myfilt, sizeof(myfilt));
19 }
20 #endif

21 sendfd = Socket(pr->sasend->sa_family, SOCK_DGRAM, 0);
// Creates the UDP socket (`sendfd`) to send the probe packets.
22 pr->sabind->sa_family = pr->sasend->sa_family;
// Sets the address family for the binding address structure.
23 sport = (getpid() & 0xffff) | 0x8000; /* our source UDP port # */
// Calculates a unique source port: uses 16 bits of PID ORed with 0x8000
// (ensures the high bit is set, aiming for a non-reserved port).
24 sock_set_port(pr->sabind, pr->salen, htons(sport));
// Sets the source port in the binding address structure.
25 Bind(sendfd, pr->sabind, pr->salen);
// Binds the calculated source port to the UDP socket (`sendfd`).
26 sig_alrm(SIGALRM);
// Calls the signal handler to initialize global state, but does not start the alarm yet.
27 seq = 0;
// Initializes the total sequence number counter.
28 done = 0;
// Initializes the exit flag.

29 for (ttl = 1; ttl <= max_ttl && done == 0; ttl++) {
// Outer loop: iterates through each Time-To-Live (TTL) value from 1 up to `max_ttl`.
30 Setsockopt(sendfd, pr->ttllevel, pr->ttloptname, &ttl, sizeof(int));
// Sets the TTL (IPv4: IP_TTL or IPv6: IPV6_UNICAST_HOPS) for outgoing UDP datagrams on `sendfd`.
31 bzero(pr->salast, pr->salen);
// Clears the `salast` address structure to ensure the router address is printed for the first probe of this TTL.
32 printf("%2d", ttl);
// Prints the current TTL number.
33 fflush(stdout);
// Flushes the output buffer so the TTL is displayed immediately.

34 for (probe = 0; probe < nprobes; probe++) {
// Inner loop: sends `nprobes` (default 3) probe packets for the current TTL.
35 rec = (struct rec *) sendbuf;
// Sets the pointer to the data payload structure within the send buffer.
36 rec->rec_seq = ++seq;
// Increments the overall sequence number and stores it in the data payload.
37 rec->rec_ttl = ttl;
// Stores the current TTL value in the data payload.
38 Gettimeofday(&rec->rec_tv, NULL);
// Records the current time in the data payload (sent timestamp).
39 sock_set_port(pr->sasend, pr->salen, htons(dport + seq));
// Sets the destination port: increments the base port by the sequence number.
40 Sendto(sendfd, sendbuf, datalen, 0, pr->sasend, pr->salen);
// Sends the UDP datagram probe packet.

41 if ((code = (*pr->recv) (seq, &tvrecv)) == -3)
// Calls the protocol-specific receive function (recv_v4 or recv_v6) and checks the return code.
42 printf("*"); /* timeout, no reply */
// If code is -3 (timeout), prints an asterisk.
43 else {
// If a reply was received (code is not -3).
44 char str[NI_MAXHOST];
// Buffer for the hostname string.
45 if (sock_cmp_addr(pr->sarecv, pr->salast, pr->salen) != 0) {
// Compares the IP address of the received ICMP message (`sarecv`) with the address of the last reply (`salast`).
46 if (getnameinfo(pr->sarecv, pr->salen, str, sizeof(str),
// If the address is new, attempt to resolve the IP address to a hostname.
47 NULL, 0, 0) == 0)
48 printf(" %s (%s)", str,
// If resolution is successful, print the hostname and IP address.
49 Sock_ntop_host(pr->sarecv, pr->salen));
50 else
// If resolution fails.
51 printf(" %s", Sock_ntop_host(pr->sarecv, pr->salen));
// Print only the IP address.
52 memcpy(pr->salast, pr->sarecv, pr->salen);
// Update `salast` with the new address for comparison on the next probe.
53 }
54 tv_sub(&tvrecv, &rec->rec_tv);
// Calculates the RTT: subtracts the sent time (`rec->rec_tv`) from the received time (`tvrecv`).
55 rtt = tvrecv.tv_sec * 1000.0 + tvrecv.tv_usec / 1000.0;
// Converts the RTT from `struct timeval` to milliseconds.
56 printf(" %.3f ms", rtt);
// Prints the calculated RTT.
57 if (code == -1) /* port unreachable; at destination */
// If the code is -1 (ICMP Port Unreachable), the destination was reached.
58 done++;
// Sets the exit flag to terminate the outer loop after this TTL.
59 else if (code >= 0)
// If the code is non-negative (another ICMP error code).
60 printf(" (ICMP %s)", (*pr->icmpcode) (code));
// Prints the error type using the protocol-specific function pointer.
61 }
62 fflush(stdout);
// Flushes the output buffer to display the results for the current probe.
63 }
64 printf("\n");
// Prints a newline after all probes for the current TTL are complete.
65 }
66 }
