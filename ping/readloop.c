1 #include "ping.h"
// Includes the local header file.
2 void
// Specifies the return type.
3 readloop(void)
// Defines the function responsible for the continuous packet reception loop.
4 {
5 int size;
// Variable to store the desired socket receive buffer size.
6 char recvbuf[BUFSIZE];
// Buffer to hold the incoming ICMP message and headers received from the socket.
7 char controlbuf[BUFSIZE];
// Buffer for ancillary data (control messages) received via `recvmsg` (used for IPv6 hop limit).
8 struct msghdr msg;
// Structure to pass and receive information with `recvmsg`.
9 struct iovec iov;
// Structure to specify the location and size of the primary data buffer.
10 ssize_t n;
// Variable to store the number of bytes read by `recvmsg`.
11 struct timeval tval;
// Structure to store the timestamp when the packet is received.

12 sockfd = Socket(pr->sasend->sa_family, SOCK_RAW, pr->icmpproto);
// Creates the raw socket: uses the address family (AF_INET or AF_INET6),
// SOCK_RAW type, and the protocol (IPPROTO_ICMP or IPPROTO_ICMPV6).
13 setuid(getuid());
// Drops superuser privileges: sets the effective user ID back to the real user ID.
// This is a security measure, as superuser privileges are only needed to *create* the raw socket.

14 if (pr->finit)
// Checks if a protocol-specific initialization function exists (i.e., init_v6 for IPv6).
15 (*pr->finit) ();
// Calls the initialization function if it is not NULL.

16 size = 60 * 1024;
// Sets the desired size of the socket receive buffer to 60 KB (61440 bytes).
17 setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
// Tries to set the socket receive buffer size. This helps prevent packet loss
// during a high volume of replies (e.g., from a broadcast/multicast ping).

18 sig_alrm(SIGALRM);
// Calls the signal handler directly to send the very first ICMP echo request
// and set the first 1-second timer for subsequent sends.

19 iov.iov_base = recvbuf;
// Sets the base address of the primary data buffer in the iovec structure.
20 iov.iov_len = sizeof(recvbuf);
// Sets the length of the primary data buffer.

21 msg.msg_name = pr->sarecv;
// Sets the pointer where the source address of the received packet will be stored.
22 msg.msg_iov = &iov;
// Sets the pointer to the array of iovec structures (only one element here).
23 msg.msg_iovlen = 1;
// Sets the number of elements in the iovec array.
24 msg.msg_control = controlbuf;
// Sets the pointer to the buffer for receiving ancillary data (control messages).

25 for (;;) {
// Starts the infinite loop for receiving packets.
26 msg.msg_namelen = pr->salen;
// Resets the size of the address buffer for the next receive operation.
27 msg.msg_controllen = sizeof(controlbuf);
// Resets the size of the ancillary data buffer.

28 n = recvmsg(sockfd, &msg, 0);
// Blocks, waiting to receive an incoming ICMP message on the raw socket,
// storing data in `recvbuf`, source address in `sarecv`, and ancillary data in `controlbuf`.

29 if (n < 0) {
// Checks for an error during `recvmsg`.
30 if (errno == EINTR)
// If the error was due to an interrupted system call (e.g., by SIGALRM).
31 continue;
// Jumps back to the beginning of the loop to call `recvmsg` again.
32 else
// For any other system error.
33 err_sys("recvmsg error");
// Prints a system error message and terminates the program.
34 }

35 Gettimeofday(&tval, NULL);
// Records the precise time of day when the packet was received, storing it in `tval`.
36 (*pr->fproc) (recvbuf, n, &msg, &tval);
// Calls the appropriate protocol-specific processing function (proc_v4 or proc_v6)
// to handle the received ICMP message, passing the received data, length, message info, and timestamp.
37 }
// End of the infinite receive loop.
38 }
