1 #include "ping.h"
// Includes the local header file.
2 void
3 send_v4(void)
// Function to build and send an ICMPv4 echo request message.
4 {
5 int len;
// Variable to store the total length of the ICMP message (header + data).
6 struct icmp *icmp;
// Pointer to the start of the ICMPv4 header within the global send buffer.

7 icmp = (struct icmp *) sendbuf;
// Casts the send buffer to an ICMP header structure.
8 icmp->icmp_type = ICMP_ECHO;
// Sets the ICMP message type to Echo Request (8).
9 icmp->icmp_code = 0;
// Sets the ICMP code field to 0 for Echo Request.
10 icmp->icmp_id = pid;
// Sets the ICMP identifier field to the truncated process ID.
11 icmp->icmp_seq = nsent++;
// Sets the ICMP sequence number to the current value of nsent, and then increments nsent.

12 memset(icmp->icmp_data, 0xa5, datalen);
// Fills the optional data portion of the ICMP message with a recognizable pattern (0xa5).
13 Gettimeofday((struct timeval *) icmp->icmp_data, NULL);
// Overwrites the first 8 bytes of the data portion with the current time of day (timestamp).
// This timestamp is used by the receiver to calculate RTT.

14 len = 8 + datalen;
// Calculates the total length of the ICMP message: 8-byte ICMP header plus data length.

15 icmp->icmp_cksum = 0;
// **Required step for checksum calculation:** Sets the checksum field to zero before calculation.
16 icmp->icmp_cksum = in_cksum((u_short *) icmp, len);
// Calls the `in_cksum` function (Internet checksum) to calculate and store the checksum
// for the ICMP header and its data payload.

17 Sendto(sockfd, sendbuf, len, 0, pr->sasend, pr->salen);
// Sends the ICMP message (in `sendbuf`) on the raw socket to the destination address (`pr->sasend`).
// Since the IP_HDRINCL option is not set, the kernel automatically prepends the IPv4 header.
18 }
