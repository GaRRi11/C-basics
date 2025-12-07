1 #include "ping.h"
// Includes the local header file.
2 void
3 proc_v6(char *ptr, ssize_t len, struct msghdr *msg, struct timeval *tvrecv)
// Function to process a received ICMPv6 message.
4 {
5 #ifdef IPV6
// Conditional compilation: only include if IPv6 is enabled.
6 double rtt;
// Variable to store the calculated Round-Trip Time (RTT) in milliseconds.
7 struct icmp6_hdr *icmp6;
// Pointer to the start of the ICMPv6 header (which is the start of the received data).
8 struct timeval *tvsend;
// Pointer to the timestamp stored in the ICMP data area (time sent).
9 struct cmsghdr *cmsg;
// Pointer for iterating through the ancillary data (control messages).
10 int hlim;
// Variable to store the received IPv6 Hop Limit.

11 icmp6 = (struct icmp6_hdr *) ptr;
// Casts the beginning of the received buffer (ptr) to an ICMPv6 header structure.
12 if (len < 8)
// Checks if the received length is less than the minimum 8-byte ICMPv6 header size.
13 return;
// If the packet is malformed (too short), ignore it.

14 if (icmp6->icmp6_type == ICMP6_ECHO_REPLY) {
// Checks if the ICMPv6 message type is an Echo Reply (the expected response).
15 if (icmp6->icmp6_id != pid)
// Checks if the ICMP identifier field matches the process's PID.
16 return;
// If the ID doesn't match, ignore the reply.
17 if (len < 16)
// Checks if the message is too short to contain the 8-byte ICMPv6 header
// AND the 8-byte timestamp we stored (8 + 8 = 16 bytes minimum).
18 return;
// If not enough data, ignore the packet.

19 tvsend = (struct timeval *) (icmp6 + 1);
// Sets the pointer to the timestamp, located immediately after the 8-byte ICMPv6 header.
20 tv_sub(tvrecv, tvsend);
// Subtracts the sent timestamp from the received timestamp (`tvrecv -= tvsend`),
// storing the RTT duration in `tvrecv`.

21 rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;
// Converts the RTT from `struct timeval` to a floating-point number in milliseconds.

22 hlim = -1;
// Initializes the Hop Limit variable to -1, indicating it has not yet been found in ancillary data.

23 for (cmsg = CMSG_FIRSTHDR(msg); cmsg != NULL;
// Starts a loop to iterate through the list of ancillary data (control messages) in the `msghdr` struct.
24 cmsg = CMSG_NXTHDR(msg, cmsg)) {
// Iteration macro: moves the pointer to the next control message.
25 if (cmsg->cmsg_level == IPPROTO_IPV6
// Checks if the control message is from the IPv6 protocol layer.
26 && cmsg->cmsg_type == IPV6_HOPLIMIT) {
// Checks if the control message type is the IPv6 Hop Limit.
27 hlim = *(u_int32_t *) CMSG_DATA(cmsg);
// Retrieves the Hop Limit value from the control message data.
28 break;
// Exits the loop once the Hop Limit is found.
29 }
30 }

31 printf("%d bytes from %s: seq=%u, hlim=",
// Prints the standard ping reply line, including the Hop Limit (hlim) placeholder.
32 len, Sock_ntop_host(pr->sarecv, pr->salen), icmp6->icmp6_seq);
// Prints the total length, source IP address (converted to string), and ICMP sequence number.
33 if (hlim == -1)
// Checks if the Hop Limit was successfully retrieved.
34 printf("???");
// Prints "???" if the ancillary data was missing or not retrieved.
35 else
36 printf("%d", hlim);
// Prints the numerical Hop Limit value.
37 printf(", rtt=%.3f ms\n", rtt);
// Prints the calculated RTT.

38 } else if (verbose) {
// If it's not an Echo Reply, check if the verbose option (-v) was specified.
39 printf("%d bytes from %s: type = %d, code = %d\n",
// Prints information about the received, non-Echo Reply ICMPv6 message.
40 len, Sock_ntop_host(pr->sarecv, pr->salen),
// Prints the length and source IP address.
41 icmp6->icmp6_type, icmp6->icmp6_code);
// Prints the ICMPv6 type and code.
42 }
43 #endif /* IPV6 */
44 }
