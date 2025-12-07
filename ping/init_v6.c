1 void
2 init_v6()
// Function to perform IPv6-specific socket initialization (called via finit pointer).
3 {
4 #ifdef IPV6
// Conditional compilation: code only runs if IPv6 support is enabled.
5 int on = 1;
// Integer variable set to 1, commonly used for enabling socket options.

6 if (verbose == 0) {
// If the program is not running in verbose mode.
7 /* install a filter that only passes ICMP6_ECHO_REPLY unless verbose */
// Comment explaining the intent: optimize by filtering out unwanted ICMPv6 messages.
8 struct icmp6_filter myfilt;
// Declares an ICMPv6 filter structure.
9 ICMP6_FILTER_SETBLOCKALL(&myfilt);
// Sets the filter to block all ICMPv6 message types by default.
10 ICMP6_FILTER_SETPASS(ICMP6_ECHO_REPLY, &myfilt);
// Explicitly sets the Echo Reply message type to *pass* the filter.
11 setsockopt(sockfd, IPPROTO_IPV6, ICMP6_FILTER, &myfilt,
// Sets the ICMPv6 filter on the socket.
12 sizeof(myfilt));
13 /* ignore error return; the filter is an optimization */
// The program can still function without the filter, so errors are ignored.
14 }

15 /* ignore error returned below; we just won’t receive the hop limit */
// Comment indicating that errors for the following options are non-fatal.
16 #ifdef IPV6_RECVHOPLIMIT
// Checks for the newer constant defined in RFC 3542.
17 /* RFC 3542 */
18 setsockopt(sockfd, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on, sizeof(on));
// Requests that the kernel return the IPv6 Hop Limit as ancillary data with incoming packets.
19 #else
// If the newer constant is not defined, check for the older one.
20 /* RFC 2292 */
21 setsockopt(sockfd, IPPROTO_IPV6, IPV6_HOPLIMIT, &on, sizeof(on));
// Requests the Hop Limit using the older constant.
22 #endif
23 #endif /* IPV6 */
24 }
