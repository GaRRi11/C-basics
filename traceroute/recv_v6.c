1 #include "trace.h"
// Includes the local header file.
2 extern int gotalarm;
// Declares the global alarm flag.
3 /*
4 *Return: -3 on timeout
5 * -2 on ICMP time exceeded in transit (caller keeps going)
6 * -1 on ICMP port unreachable (caller is done)
7 * >=0 return value is some other ICMP unreachable code
8 */
// Function documentation defining return codes.
9 int
10 recv_v6(int seq, struct timeval *tv)
// Function to receive and process ICMPv6 replies.
11 {
12 #ifdef IPV6
// Conditional compilation block.
13 int hlen2, icmp6len, ret;
// `hlen2`: inner IP header length; `icmp6len`: ICMPv6 message length; `ret`: return value.
14 ssize_t n;
// Variable to store the number of bytes read.
15 socklen_t len;
// Variable for the length of the socket address structure.
16 struct ip6_hdr *hip6;
// Pointer to the inner (encapsulated) IPv6 header.
17 struct icmp6_hdr *icmp6;
// Pointer to the ICMPv6 header.
18 struct udphdr *udp;
// Pointer to the UDP header (encapsulated).
19 gotalarm = 0;
// Clears the alarm flag.
20 alarm(3);
// Sets a 3-second timer.

21 for (;;) {
// Starts the infinite receive loop.
22 if (gotalarm)
23 return (-3); /* alarm expired */
// Returns -3 if timeout occurred.
24 len = pr->salen;
// Sets the address length.
25 n = recvfrom(recvfd, recvbuf, sizeof(recvbuf), 0, pr->sarecv, &len);
// Blocks, waiting to receive an ICMPv6 message (data starts with ICMPv6 header on raw IPv6 socket).

26 if (n < 0) {
// Checks for an error during `recvfrom`.
27 if (errno == EINTR)
28 continue;
// Continue if interrupted by a signal.
29 else
30 err_sys("recvfrom error");
// Terminate on other system errors.
31 }

32 icmp6 = (struct icmp6_hdr *) recvbuf; /* ICMP header */
// Points to the ICMPv6 header (start of the received data).
33 if ( (icmp6len = n) < 8)
// Calculates the ICMPv6 message length and checks if it's less than the 8-byte header minimum.
34 continue;
// If malformed, ignore.

35 if (icmp6->icmp6_type == ICMP6_TIME_EXCEEDED &&
// Checks for ICMPv6 Time Exceeded message type (100).
36 icmp6->icmp6_code == ICMP6_TIME_EXCEED_TRANSIT) {
// Checks for the specific code: Time Exceeded in Transit (0).
37 if (icmp6len < 8 + sizeof(struct ip6_hdr) + 4)
// Checks if the payload contains enough data for 8-byte ICMPv6 header + inner IPv6 header (40 bytes) + 4 bytes of UDP header.
38 continue;
// If not enough data, ignore.
39 hip6 = (struct ip6_hdr *) (recvbuf + 8);
// Points to the inner IPv6 header (after the 8-byte ICMPv6 header).
40 hlen2 = sizeof(struct ip6_hdr);
// The IPv6 header has a fixed length of 40 bytes.
41 udp = (struct udphdr *) (recvbuf + 8 + hlen2);
// Points to the inner UDP header.
42 if (hip6->ip6_nxt == IPPROTO_UDP &&
// Checks if the next header of the inner datagram was UDP.
43 udp->uh_sport == htons(sport) &&
// Checks for matching source port.
44 udp->uh_dport == htons(dport + seq))
// Checks for matching destination port/sequence number.
45 ret = -2;
// Intermediate router hit.
46 break;
// Exits the receive loop.

47 } else if (icmp6->icmp6_type == ICMP6_DST_UNREACH) {
// Checks for ICMPv6 Destination Unreachable message type (1).
48 if (icmp6len < 8 + sizeof(struct ip6_hdr) + 4)
// Checks for minimum encapsulated header size.
49 continue;
50 hip6 = (struct ip6_hdr *) (recvbuf + 8);
// Points to the inner IPv6 header.
51 hlen2 = sizeof(struct ip6_hdr);
// Inner IPv6 header length.
52 udp = (struct udphdr *) (recvbuf + 8 + hlen2);
// Points to the inner UDP header.
53 if (hip6->ip6_nxt == IPPROTO_UDP &&
// Checks if inner protocol was UDP.
54 udp->uh_sport == htons(sport) &&
// Checks for matching source port.
55 udp->uh_dport == htons(dport + seq)) {
// Checks for matching destination port/sequence number.
56 if (icmp6->icmp6_code == ICMP6_DST_UNREACH_NOPORT)
// If the code is No Port Unreachable (success).
57 ret = -1; /* have reached destination */
// Destination reached.
58 else
59 ret = icmp6->icmp6_code; /* 0, 1, 2, ... */
// Otherwise, it's another Destination Unreachable error code.
60 break;
// Exits the receive loop.
61 }
62 } else if (verbose) {
// If the verbose flag is set.
63 printf(" (from %s: type = %d, code = %d)\n",
// Prints information about other received ICMPv6 errors.
64 Sock_ntop_host(pr->sarecv, pr->salen),
65 icmp6->icmp6_type, icmp6->icmp6_code);
66 }
67 /* Some other ICMP error, recvfrom() again */
// Continue the loop for other ICMP errors.
68 }
69 alarm(0);
// Turns off the alarm.
70 Gettimeofday(tv, NULL); /* get time of packet arrival */
// Records the arrival time.
71 return (ret);
// Returns the status code.
72 #endif
73 }
