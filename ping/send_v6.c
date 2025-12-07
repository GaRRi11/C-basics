1 #include "ping.h"
// Includes the local header file.
2 void
3 send_v6()
// Function to build and send an ICMPv6 echo request message.
4 {
5 #ifdef IPV6
// Conditional compilation: code only runs if IPv6 is enabled.
6 int len;
// Variable to store the total length of the ICMPv6 message (header + data).
7 struct icmp6_hdr *icmp6;
// Pointer to the start of the ICMPv6 header within the global send buffer.

8 icmp6 = (struct icmp6_hdr *) sendbuf;
// Casts the send buffer to an ICMPv6 header structure.
9 icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
// Sets the ICMPv6 message type to Echo Request (128).
10 icmp6->icmp6_code = 0;
// Sets the ICMPv6 code field to 0 for Echo Request.
11 icmp6->icmp6_id = pid;
// Sets the ICMPv6 identifier field to the truncated process ID.
12 icmp6->icmp6_seq = nsent++;
// Sets the ICMPv6 sequence number to the current value of nsent, and then increments nsent.

13 memset((icmp6 + 1), 0xa5, datalen);
// Fills the optional data portion (starting immediately after the header) with a pattern.
14 Gettimeofday((struct timeval *) (icmp6 + 1), NULL);
// Overwrites the first 8 bytes of the data portion with the current timestamp.

15 len = 8 + datalen;
// Calculates the total length of the ICMPv6 message: 8-byte ICMPv6 header plus data length.

16 Sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen);
// Sends the ICMPv6 message on the raw socket to the destination address.

17 /* kernel calculates and stores checksum for us */
// **Crucial difference from IPv4:** The ICMPv6 checksum calculation includes the IPv6 pseudo-header,
// which requires the source address. Since the kernel selects the source address,
// the kernel must calculate the ICMPv6 checksum itself.
18 #endif /* IPV6 */
19 }
