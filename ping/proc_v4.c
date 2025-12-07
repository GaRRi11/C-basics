1 #include "ping.h"
// Includes the local header file.
2 void
3 proc_v4(char *ptr, ssize_t len, struct msghdr *msg, struct timeval *tvrecv)
// Function to process a received ICMPv4 message.
4 {
5 int hlen1, icmplen;
// `hlen1`: length of the IPv4 header; `icmplen`: length of the ICMP message (header + data).
6 double rtt;
// Variable to store the calculated Round-Trip Time (RTT) in milliseconds.
7 struct ip *ip;
// Pointer to the start of the IPv4 header (at the beginning of the buffer).
8 struct icmp *icmp;
// Pointer to the start of the ICMPv4 header.
9 struct timeval *tvsend;
// Pointer to the timestamp stored in the ICMP data area (time sent).

10 ip = (struct ip *) ptr;
// Casts the beginning of the received buffer (ptr) to an IPv4 header structure.
11 hlen1 = ip->ip_hl << 2;
// Calculates the length of the IPv4 header: `ip_hl` is the header length in 32-bit words;
// shifting left by 2 (or multiplying by 4) converts it to bytes.

12 if (ip->ip_p != IPPROTO_ICMP)
// Checks if the protocol field in the IPv4 header is not ICMP.
13 return;
// If it's not ICMP, ignore the packet and return.

14 icmp = (struct icmp *) (ptr + hlen1);
// Sets the ICMP pointer to the location immediately following the IPv4 header.
15 if ((icmplen = len - hlen1) < 8)
// Calculates the length of the ICMP message (total length - IPv4 header length).
// Checks if the ICMP message is shorter than the minimum 8-byte ICMP header.
16 return;
// If the packet is malformed (too short), ignore it.

17 if (icmp->icmp_type == ICMP_ECHOREPLY) {
// Checks if the ICMP message type is an Echo Reply (the expected response).
18 if (icmp->icmp_id != pid)
// Checks if the ICMP identifier field matches the process's PID (to ignore replies from other ping instances).
19 return;
// If the ID doesn't match, ignore the reply.
20 if (icmplen < 16)
// Checks if the ICMP message is too short to contain the 8-byte ICMP header
// AND the 8-byte timestamp we stored (8 + 8 = 16 bytes minimum).
21 return;
// If not enough data, ignore the packet.

22 tvsend = (struct timeval *) icmp->icmp_data;
// Sets the pointer to the `timeval` structure stored at the beginning of the ICMP data area.
23 tv_sub(tvrecv, tvsend);
// Subtracts the *sent* timestamp from the *received* timestamp (`tvrecv -= tvsend`).
// The result in `tvrecv` is the Round-Trip Time (RTT) duration.

24 rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;
// Converts the RTT from `struct timeval` (seconds + microseconds) to a floating-point number in milliseconds.

25 printf("%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms\n",
// Prints the standard ping reply line.
26 icmplen, Sock_ntop_host(pr->sarecv, pr->salen),
// Prints the ICMP message length and the source IP address (converted to a string).
27 icmp->icmp_seq, ip->ip_ttl, rtt);
// Prints the ICMP sequence number, the IPv4 TTL (Time-To-Live), and the calculated RTT.

28 } else if (verbose) {
// If it's not an Echo Reply, check if the verbose option (-v) was specified.
29 printf("%d bytes from %s: type = %d, code = %d\n",
// Prints information about the received, non-Echo Reply ICMP message.
30 icmplen, Sock_ntop_host(pr->sarecv, pr->salen),
// Prints length and source IP address.
31 icmp->icmp_type, icmp->icmp_code);
// Prints the ICMP type and code (e.g., Destination Unreachable, Time Exceeded).
32 }
33 }
