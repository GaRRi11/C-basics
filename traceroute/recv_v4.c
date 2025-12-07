1 #include "trace.h"
// Includes the local header file.
2 extern int gotalarm;
// Declares the global flag used by the signal handler.
3 /*
4 *Return: -3 on timeout
5 * -2 on ICMP time exceeded in transit (caller keeps going)
6 * -1 on ICMP port unreachable (caller is done)
7 * >=0 return value is some other ICMP unreachable code
8 */
// Function documentation defining return codes.
9 int
10 recv_v4(int seq, struct timeval *tv)
// Function to receive and process ICMPv4 replies for the given sequence number.
11 {
12 int hlen1, hlen2, icmplen, ret;
// `hlen1`: outer IP header length; `hlen2`: inner IP header length; `icmplen`: ICMP message length; `ret`: return value.
13 socklen_t len;
// Variable for the length of the socket address structure.
14 ssize_t n;
// Variable to store the number of bytes read.
15 struct ip *ip, *hip;
// `ip`: pointer to the outer IPv4 header; `hip`: pointer to the inner (encapsulated) IPv4 header.
16 struct icmp *icmp;
// Pointer to the ICMPv4 header.
17 struct udphdr *udp;
// Pointer to the UDP header (encapsulated in the ICMP message).
18 gotalarm = 0;
// Clears the alarm flag before starting the receive loop.
19 alarm(3);
// Sets a 3-second timer for the probe timeout.

20 for (;;) {
// Starts the infinite loop to receive packets until a relevant one is found or a timeout occurs.
21 if (gotalarm)
// Checks if the `sig_alrm` handler has set the flag (i.e., the 3-second alarm expired).
22 return (-3); /* alarm expired */
// Returns -3 if a timeout occurred.
23 len = pr->salen;
// Sets the address length for `recvfrom`.
24 n = recvfrom(recvfd, recvbuf, sizeof(recvbuf), 0, pr->sarecv, &len);
// Blocks, waiting to receive an ICMP message on the raw socket, storing the source address in `sarecv`.

25 if (n < 0) {
// Checks for an error during `recvfrom`.
26 if (errno == EINTR)
// If the error was due to an interrupted system call (e.g., by the SIGALRM signal).
27 continue;
// Jumps back to the beginning of the loop to check `gotalarm` and call `recvfrom` again.
28 else
29 err_sys("recvfrom error");
// For any other system error, terminate.
30 }

31 ip = (struct ip *) recvbuf; /* start of IP header */
// Points to the start of the outer IPv4 header (raw socket returns IP header).
32 hlen1 = ip->ip_hl << 2; /* length of IP header */
// Calculates the length of the outer IPv4 header in bytes.
33 icmp = (struct icmp *) (recvbuf + hlen1); /* start of ICMP header */
// Points to the ICMP header, located after the outer IPv4 header.
34 if ( (icmplen = n - hlen1) < 8)
// Calculates the ICMP message length and checks if it's less than the 8-byte minimum header size.
35 continue;
// If malformed, ignore and receive again.

36 if (icmp->icmp_type == ICMP_TIMXCEED &&
// Checks for ICMP Time Exceeded message type.
37 icmp->icmp_code == ICMP_TIMXCEED_INTRANS) {
// Checks for the specific code: Time Exceeded in Transit.
38 if (icmplen < 8 + sizeof(struct ip))
// Checks if the payload contains at least the 8-byte ICMP header + inner IP header minimum size.
39 continue;
// If not enough data for the inner IP header, ignore.
40 hip = (struct ip *) (recvbuf + hlen1 + 8);
// Points to the inner IPv4 header (after outer IP + ICMP header).
41 hlen2 = hip->ip_hl << 2;
// Calculates the length of the inner IPv4 header.
42 if (icmplen < 8 + hlen2 + 4)
// Checks if the payload contains enough data to reach the inner UDP ports (8 ICMP + hlen2 IP + 4 bytes of UDP header).
43 continue;
// If not enough data for UDP ports, ignore.
44 udp = (struct udphdr *) (recvbuf + hlen1 + 8 + hlen2);
// Points to the inner UDP header.
45 if (hip->ip_p == IPPROTO_UDP &&
// Checks if the protocol of the inner datagram was UDP.
46 udp->uh_sport == htons(sport) &&
// Checks if the source port matches our program's unique source port.
47 udp->uh_dport == htons(dport + seq)) {
// Checks if the destination port matches the port we sent with this sequence number.
48 ret = -2; /* we hit an intermediate router */
// If all checks pass, it's a valid reply from an intermediate router.
49 break;
// Exits the receive loop.
50 }
51 } else if (icmp->icmp_type == ICMP_UNREACH) {
// Checks for ICMP Destination Unreachable message type.
52 if (icmplen < 8 + sizeof(struct ip))
// Checks for minimum encapsulated IP header size.
53 continue;
54 hip = (struct ip *) (recvbuf + hlen1 + 8);
// Points to the inner IPv4 header.
55 hlen2 = hip->ip_hl << 2;
// Calculates inner IPv4 header length.
56 if (icmplen < 8 + hlen2 + 4)
// Checks for minimum encapsulated UDP port size.
57 continue;
58 udp = (struct udphdr *) (recvbuf + hlen1 + 8 + hlen2);
// Points to the inner UDP header.
59 if (hip->ip_p == IPPROTO_UDP &&
// Checks if the inner protocol was UDP.
60 udp->uh_sport == htons(sport) &&
// Checks for matching source port.
61 udp->uh_dport == htons(dport + seq)) {
// Checks for matching destination port/sequence number.
62 if (icmp->icmp_code == ICMP_UNREACH_PORT)
// If the code is Port Unreachable.
63 ret = -1; /* have reached destination */
// Destination reached (success).
64 else
65 ret = icmp->icmp_code; /* 0, 1, 2, ... */
// Otherwise, it's another Destination Unreachable error code.
66 break;
// Exits the receive loop.
67 }
68 }
69 if (verbose) {
// If the verbose flag is set.
70 printf(" (from %s: type = %d, code = %d)\n",
// Prints information about other received ICMP errors (those not related to our probe).
71 Sock_ntop_host(pr->sarecv, pr->salen),
72 icmp->icmp_type, icmp->icmp_code);
73 }
74 /* Some other ICMP error, recvfrom() again */
// Comment indicating that the loop should continue if it wasn't a relevant ICMP error.
75 }
76 alarm(0);
// Turns off the 3-second alarm before returning.
77 Gettimeofday(tv, NULL); /* get time of packet arrival */
// Records the time of arrival in the provided `tv` structure.
78 return (ret);
// Returns the status code (-2, -1, or an ICMP code >= 0).
79 }
