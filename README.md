## Variables Live in Memory

Every variable has a value and a memory address.

int x = 10;   // value

&x;           // memory location of x

## What Pointers Really Are

A pointer stores the memory address of another variable.

int x = 10;

int *p = &x; 


p is the address (example: 0x83498A).

*p is the value living at that address.

C knows int is 4 bytes, so *p reads 4 bytes starting at that address → 10.

Rule:
Pointer type must match the variable type it points to, otherwise C will read the wrong number of bytes.

USAGE EXAMPLE:

Pointers let you access/modify memory indirectly:

void change(int x) {

    x = 99; // DOES NOT affect original
    
}

void change(int *p) {

    *p = 99; // changes original
    
}

Pointers allow dynamic memory (malloc):

int *arr = malloc(100 * sizeof(int));

Pointers let you walk through arrays efficiently

uint8_t *p = buf;

p++; // move to next byte

Pointers allow casting raw bytes into structs (packet parsing)

struct Header *h = (struct Header*)buf;

Some data structures require pointers

linked lists

trees

hash tables

graphs



pointer[index] = *(pointer + index)

This means:

pb[0] → the byte at address pb

pb[1] → the byte at address pb + 1

pb[2] → the byte at address pb + 2


## Pointer Type Defines Read Size

The pointer's type tells C how many bytes to read.

int x = 10;

uint8_t *p = &x;   // unsafe


uint8_t* reads only 1 byte from the 4-byte integer — wrong unless intentionally reading raw bytes.

## How Pointers Work With Arrays

An array name decays to the address of its first element.

uint8_t array[4] = {1, 2, 3, 4};

uint8_t *p = array; 


p = &array[0]

*p reads the first element → 1

Pointer arithmetic:

*p       // 1

*(p + 2) // 3 (third element, 2 bytes forward)


Because uint8_t is 1 byte, pointer moves by 1 byte each step.

## Pointer Arithmetic Moves in Type-Sized Steps

Adding 1 to a pointer moves by the size of the pointed type.

uint32_t *p;

p + 1;  // moves 4 bytes forward

uint8_t *b;

b + 1;  // moves 1 byte forward


Example:

uint32_t arr[2] = {100, 200};

uint32_t *p = arr;

*(p + 1);  // 200

## Struct Memory Layout

A struct is a group of variables placed sequentially in memory.

struct Packet {

    uint8_t type;      // 1 byte
    
    uint16_t length;   // 2 bytes
    
};


Fields are laid out next to each other.

Padding bytes may be inserted between fields for alignment.

## sizeof()

sizeof tells you exactly how many bytes something uses.

sizeof(int);        // usually 4

## Network Byte Order

Network byte order is always big-endian.

Multi-byte values must be converted:

htons() → 16-bit

htonl() → 32-bit

uint16_t port = 80;

uint16_t net_port = htons(port);

## Walking Through a Packet

Interpret raw network bytes using a struct pointer.

Raw data:

uint8_t buf[1500];


Cast to struct:

struct Header *h = (struct Header*)buf;


Access fields:

h->version → byte at offset 0

h->type → byte at offset 1

h->length → 2 bytes at offset 2

This is walking the packet by reading its structured fields. 


NOTES:

1.addresses must be printed like this 


    printf("%p\n", (void*)&x); // address
    printf("%p\n", (void*)p);  // same address

    
2.Endianness is how multi-byte numbers are stored in memory. MSB (most significant byte) contributes the most, LSB the least. Hex notation (e.g., 0x12345678) is always MSB→LSB and does not reflect memory layout. Big-endian stores MSB at the lowest address, little-endian stores LSB at the lowest address. Pointers read memory byte by byte, so the first byte depends on endianness. Networking always uses big-endian, so conversions (htons/htonl) may be needed. Value stays the same; only byte order changes.

3. FD is an int — correct. A file descriptor is just a small integer that the kernel uses to track open resources. Standard FDs when a process starts:

| FD | Name   | Description     |
| -- | ------ | --------------- |
| 0  | STDIN  | standard input  |
| 1  | STDOUT | standard output |
| 2  | STDERR | standard error  |

These are automatically opened and managed by the kernel for every process. ✅

System calls for I/O

write(int fd, const void *buf, size_t count) → writes count bytes from buf to the file/socket/resource identified by fd.

Example: fd = 1 → stdout (process outputs data).

read(int fd, void *buf, size_t count) → reads up to count bytes into buf from the file/socket/resource identified by fd.

Example: fd = 0 → stdin (process reads input).

4. Sockets 

What are sockets?

Sockets are used to send and receive data between processes (locally) or over a network (between devices).

Sockets allow TCP and UDP.

Other protocols are built on top of these two.

A socket itself is a file descriptor, so we can read(), write(), and close() it like a normal FD.

Creating a Socket

Template:

socket(domain, type, protocol)


Example:

int sockfd = socket(AF_INET, SOCK_STREAM, 0);

Parameters

domain → AF_INET for IPv4, AF_INET6 for IPv6, AF_UNIX for local communication

type → SOCK_STREAM for TCP, SOCK_DGRAM for UDP

protocol → 0 (kernel chooses default protocol for that type)

socket() returns a file descriptor (>= 0) or -1 on error.

Creating a TCP Client

To connect to a server we need to prepare a sockaddr_in structure.

Here we save IP + PORT for IPv4 format.

Steps

1. Create struct
   
struct sockaddr_in addr;

3. Set address family
   
addr.sin_family = AF_INET;


This makes sockaddr_in a valid IPv4 address struct.

3. Set destination port
   
addr.sin_port = htons(8080);


We convert the port to network byte order (big-endian).

4. Convert destination IP to 32-bit binary
   
inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);


This converts the loopback IP from string to a 32-bit IPv4 address.

5. Connect
   
connect(sock, (struct sockaddr*)&addr, sizeof(addr));


sock → the local socket FD

addr → server IP + port

sizeof(addr) → lets OS know the structure size

If connection succeeds, the TCP 3-way handshake happens:

SYN →
← SYN + ACK
ACK →

Sending Data

char msg[] = "hello server";


Create message we want to send to server.

send(sock, msg, strlen(msg), 0);


sock → the connected socket

msg → pointer to bytes

strlen(msg) → number of bytes (11, without '\0')

0 → no special flags

Closing

close(sock);


Closes the connection.

This sends FIN packet to the server.


5. RAW PACKET

✅ Raw Packets – Craft ARP and Send to LAN

Choosing interface

const char *iface = "eth0";   // specify interface

✅ Why AF_PACKET + SOCK_RAW

When crafting raw packets where we want to manually assign src MAC, src IP, and all other fields, we must use:

AF_PACKET, SOCK_RAW


If we don't, the kernel will override IP and MAC values.

✅ Creating raw socket

int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));


AF_PACKET: allows sending full Ethernet frames

SOCK_RAW: gives raw access to link-layer

htons(ETH_P_ARP): tell kernel we are interested in ARP protocol (protocol filter)

ETH_P_ALL can be used to capture or send all protocols

✅ Interface index

C knows interfaces by index (integer IDs):

eth0 → 2

wlan0 → 3


Use SIOCGIFINDEX ioctl to get index from interface name.

✅ Crafting ARP packet fields

Source MAC

unsigned char src_mac[6];


Store the source MAC for the packet (can be random 6 bytes).

Source IP

uint32_t src_ip_netorder;


Store source IPv4 in network byte order.

Target IP

uint32_t target_ip_netorder;

✅ Ethernet + ARP frame layout

Ethernet header: 14 bytes

- dst MAC (6)
  
- src MAC (6)
  
- ethertype (2) -> 0x0806 for ARP

ARP payload (request): 28 bytes

- htype (2) = 1
- ptype (2) = 0x0800
- hlen (1) = 6
- plen (1) = 4
- opcode (2) = 1 (request)
- sender MAC (6)
- sender IP (4)
- target MAC (6) = zeros
- target IP (4)

Total = 42 bytes

✅ Frame buffer

unsigned char frame[42];


Buffer that holds full Ethernet frame.

All indexes refer to offsets in this array.

✅ Destination MAC

unsigned char dest_mac[6] = {ff:ff:ff:ff:ff:ff};


Broadcast to forward packet to everyone in LAN.

✅ EtherType

frame[12] = 0x08;  
frame[13] = 0x06;

✅ ARP header fields

frame[14] = 0x00; frame[15] = 0x01;    // HTYPE = 1 (Ethernet)
frame[16] = 0x08; frame[17] = 0x00;    // PTYPE = 0x0800 (IPv4)
frame[18] = 6;                         // HLEN = 6
frame[19] = 4;                         // PLEN = 4
frame[20] = 0x00; frame[21] = 0x01;    // opcode = 1 (request)

memcpy(frame + 22, src_mac, 6);        // sender MAC
memcpy(frame + 28, &src_ip_netorder, 4);  // sender IP
memset(frame + 32, 0x00, 6);           // target MAC = unknown
memcpy(frame + 38, &target_ip_netorder, 4); // target IP

✅ sockaddr_ll setup

struct sockaddr_ll socket_address;


Contains:

family = AF_PACKET

interface index

hardware address length = 6

destination MAC address

✅ Sending the packet

ssize_t bytes_sent = sendto(sock, frame, sizeof(frame), 0,
                (struct sockaddr*)&socket_address,
                sizeof(socket_address));


If bytes_sent > 0 → successfully sent.

✅ Closing

close(sock);

✅ Sniff all packets in LAN

Create sniffer socket

int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));


AF_PACKET → capture raw L2 frames

SOCK_RAW → get entire Ethernet frame (header + payload)

ETH_P_ALL → capture every protocol (ARP, IPv4, IPv6, etc)

Bind to interface

int ifindex = ifr.ifr_ifindex;


Bind socket to correct interface index.

Sniff loop
while (1) {
    recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL);
}


Parse offsets in buffer and print in human-readable form.

✅ Changes if using IPv6 instead of IPv4

Sender (raw crafting)

EtherType = 0x86DD

No ARP → use ICMPv6 ND

16-byte IPs

IPv6 header = 40 bytes

Must compute ICMPv6 checksum

Address conversion uses AF_INET6

Sniffer

Detect IPv6 using EtherType 0x86DD

Parse 40-byte header

IPs are 16 bytes printed via inet_ntop(AF_INET6)

No ARP → instead ICMPv6 ND







---------------------------------------------------------------- C NETWORKING

UNIX

chapter 3 - 7 - 14 - 17 - 27 - 28 - 29

PF_PACKET (AF_PACKET) — Kernel will NOT override anything

af_packet not overriden just fails if not specified l2 and af_intet overriden always



ROADMAP

protocols - rules of networking between devices. protocol can have different level of rules, like ethenet cable voltage(low level) or how JPEG image gets formatted (high level)

OSI LAYER MODEL

osi defines this levels clearly, every upper layer is built on lower level layer and uses it to operate

  7. APPLICATION - APPLICATIONS WITH UI, this layer uses all 6 lower layers.

  6. PRESENTATION - at this level, data structure is defined, data encoding, serialization, encryption.

  5. SESSION LAYER - uses transport layer and adds methods to establish, suspend, checkpoint, resume and terminate dialogs.

  4 TRANSPORT - TCP/UDP methods to deliver data between hosts, this methods split up data, recombining it, ensuring it arrived, etc...

  3 NETWORK - transmit packets in different networks, this layer is used by internet protocol.

  2 DATA LINK - It deals with protocols for directly communicating between two nodes. It defines how a direct message between nodes starts and ends (framing), error detection and correction, and flow control. 802.11(WIFI) or Ethernet frame(WIRED)

  1 PHYSICAL - ethernet cable voltage, wifi radio frequency. light flashes over an optic fiber. RADIO WAVES(WIFI) or SIGNALS(WIRED)


each upper layer has its own message only and has info about what to send to next lower layer, on each layer more info is added to data about the next hop. thats called encapsulation. 

when packet reaches dest then decapsulation starts. each layer reads and removes its header.


TCP OR UDP always required for connection in intrnet. because IP required Layer 4 transport layer.

transmitting http with tcp is handled by os.



TCP/IP LAYER MODEL

  4. PROCCESS/APPLICATION -  The process/application layer is where protocols such as HTTP, SMTP, and FTP are implemented.

  3. HOST TO HOST - TCP/UDP

  2. INTERNET - This layer deals with the concerns of addressing packetsand routing them over multiple

  1 NETWORK ACCESS - ethernet cable voltage, wifi radio frequency. light flashes over an optic fiber. RADIO WAVES(WIFI) or SIGNALS(WIRED)




LAN PROTOCOLS AND THEIR LAYERS:

| Protocol                                        | Layer                                                                | Description                                                            |
| ----------------------------------------------- | -------------------------------------------------------------------- | ---------------------------------------------------------------------- |
| **ARP** (Address Resolution Protocol)           | **Layer 2 (Link Layer)**                                             | Resolves IPv4 → MAC. Runs directly over Ethernet. Not an IP protocol.  |
| **NDP** (NS/NA, RS/RA, Redirect, etc. for IPv6) | **Layer 3 (Network Layer)**                                          | Neighbor Discovery Protocol. Built on **ICMPv6** which runs over IPv6. |
| **DHCPv4**                                      | **Layer 7 (Application)** but carried by **UDP (Layer 4)** over IPv4 | Used for IPv4 address assignment.                                      |
| **DHCPv6**                                      | **Layer 7 (Application)** carried by **UDP** over IPv6               | IPv6 version of DHCP.                                                  |
| **ICMPv4**                                      | **Layer 3 (Network)** over IPv4                                      | Ping, errors, etc.                                                     |
| **ICMPv6**                                      | **Layer 3 (Network)** over IPv6                                      | Required for IPv6. NDP uses this.                                      |






SOCKET INTRODUCTION - 91



 Most socket functions require a pointer to a socket address structure as an argument.
 Each supported protocol suite defines its own socket address structure. The names of
 these structures begin with sockaddr_ and end with a unique suffix for each protocol
 suite.


sockaddr_in - IPv4 socket address structure.

struct in_addr {
    in_addr_t s_addr;
};

 in_addr represents 32-bit IPv4 addr. s_addr: A 32-bit integer storing the IPv4 address in network byte order (big-endian). this represents just an IPv4 addr in big-endian format. not port number or anything else.


sockaddr_in. WHEN SENDING THIS STRUCTURE IN NETWORK ONLY IP ADDR AND PORT FIELDS ARE CONVERTED TO BIG-ENDIAN AND SENT OTHER FIELDS STAY HOST LOCAL NOT SENT IN NETWORK.

struct sockaddr_in {
    uint8_t sin_len;       // length (BSD only) Not all vendors support
    sa_family_t sin_family;// AF_INET
    in_port_t sin_port;    // TCP/UDP port in network byte order
    struct in_addr sin_addr;// IPv4 address
    char sin_zero[8];      // padding
};


sockaddr_in represents full full IPv4 socket address, including:

sin_len: As a developer, you usually don’t need to touch sin_len unless you’re doing advanced things like routing sockets.
IPv6 Note: IPv6 implementations often define SIN6_LEN if the length field exists.

IP address (sin_addr)

Port number (sin_port)

Address family (sin_family)

Used in network functions: bind(), connect(), sendto(), recvfrom().

LAYER 4 SOCKET: socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)

LAYER 3 SOCKET: socket(AF_INET, SOCK_RAW, IPPROTO_RAW)

LAYER 2 SOCKET: socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))



POSIX only requires three members in struct sockaddr_in:

sin_family – Address family (e.g., AF_INET for IPv4)

sin_port – 16-bit TCP/UDP port number (network byte order)

sin_addr – 32-bit IPv4 address (network byte order)



sin_zero padding

Almost all implementations include sin_zero[8].

Purpose:

Ensures struct sockaddr_in is at least 16 bytes (matches struct sockaddr size).

Provides memory alignment and compatibility with older code that expects 16-byte socket addresses.


POSIX defines minimum requirements for socket programming.

POSIX datatypes for socket members:

| Member            | POSIX Type    | Requirements / Notes                                                      |
| ----------------- | ------------- | ------------------------------------------------------------------------- |
| `sin_family`      | `sa_family_t` | Any unsigned integer. Usually 8 bits if `sin_len` exists, 16 bits if not. |
| `sin_port`        | `in_port_t`   | Unsigned integer of **at least 16 bits** (usually `uint16_t`).            |
| `sin_addr.s_addr` | `in_addr_t`   | Unsigned integer of **at least 32 bits** (usually `uint32_t`).            |


Other related POSIX datatypes

| Type        | Description                  | Header           |
| ----------- | ---------------------------- | ---------------- |
| `int8_t`    | Signed 8-bit integer         | `<sys/types.h>`  |
| `uint8_t`   | Unsigned 8-bit integer       | `<sys/types.h>`  |
| `int16_t`   | Signed 16-bit integer        | `<sys/types.h>`  |
| `uint16_t`  | Unsigned 16-bit integer      | `<sys/types.h>`  |
| `int32_t`   | Signed 32-bit integer        | `<sys/types.h>`  |
| `uint32_t`  | Unsigned 32-bit integer      | `<sys/types.h>`  |
| `socklen_t` | Length of a socket structure | `<sys/socket.h>` |




sa_family_t → Identifies the address family.

For IPv4: AF_INET

For IPv6: AF_INET6


socklen_t → Represents the length of a socket address structure (used in bind(), accept(), etc.).

Usually a 32-bit unsigned integer.



sin_addr(IPv4 addr) and sin_port(port  number) must be stored in big-endian order before sent.

convert from host to big-endian - htons(), htonl()

convert big-endian to host - ntohs(), ntohl()



serv.sin_addr → accesses the in_addr structure

serv.sin_addr.s_addr → accesses the 32-bit integer storing the IPv4 address



Modern systems define in_addr as a structure with a single in_addr_t member.


different protocol families (IPv4,IPv6,local) have different structures struct_sockaddr_in, struct_sockaddr_in6, struct_sockaddr_un.socket functions which are bind() connect() sendto() need to work with any protocol family. because of that generic socket address is used, named struct sockaddr.

struct sockaddr {
    uint8_t sa_len;       // BSD: length of the structure
    sa_family_t sa_family; // Address family: AF_xxx
    char sa_data[14];     // Protocol-specific address (port + IP for IPv4)
};

sa_family tells the kernel which protocol this address is for.

sa_data is a placeholder for the actual protocol-specific data.

For IPv4, it stores sin_port + part of sin_addr.

For other protocols, it’s interpreted differently.

The kernel receives a pointer to struct sockaddr.

It checks sa_family to know which protocol-specific structure it should treat the pointer as.

This is how one function can handle multiple protocol families.





IPv6 Socket Address Structure

struct in6_addr {
    uint8_t s6_addr[16];  // 128-bit IPv6 address
};


in6_addr holds a 16-byte IPv6 address in network byte order (big endian).

struct sockaddr_in6 {
    uint8_t       sin6_len;        // length of this structure
    sa_family_t   sin6_family;     // AF_INET6
    in_port_t     sin6_port;       // transport layer port number (network byte order)
    uint32_t      sin6_flowinfo;   // flow label + reserved bits
    struct in6_addr sin6_addr;     // IPv6 address (network byte order)
    uint32_t      sin6_scope_id;   // identifies the scope zone/interface
};

actual IPv6 socket address structure.

sin6_scope_id identifies which interface an IPv6 address belongs to. this required only for interface unique LLA s. not for GUA s. sin6_scope_id tells kernel which interface the LLA belogns to. every interface has its own LLA, So the kernel needs to know which interface’s link-local network you want. BUT for GUA specifing interface index not needed because GUA is valid for all interfaces.

addr.sin6_scope_id = if_nametoindex("eth0"); 

assigns sin6_scope_id the index of the interface. for example: 2 or 3.

port and addr fields converted to big-endian as always.

real example of defining sockaddr_in6 with real values:

#include <string.h>
#include <netinet/in.h>
#include <net/if.h>

struct sockaddr_in6 addr;
memset(&addr, 0, sizeof(addr));     // Always clear the struct

addr.sin6_family   = AF_INET6;      // Address family = IPv6
addr.sin6_port     = htons(8080);   // TCP/UDP port in network byte order
inet_pton(AF_INET6,
          "fe80::1234:abcd:5678:9abc",
          &addr.sin6_addr);         // Store IPv6 address (network byte order)

addr.sin6_flowinfo = 0;             // Not used normally
addr.sin6_scope_id = if_nametoindex("eth0");   // REQUIRED for link-local



sockaddr_storage

this is created for ensuring it can store any type of addr. (address only not other details like port and etc..).  example:

/* illustrative — actual system definition is implementation dependent */
#include <stdint.h>
#include <sys/types.h>    /* for sa_family_t */
#include <stddef.h>

#define _SS_SIZE 128               /* total size (common choice) */
#define _SS_ALIGNMENT sizeof(long long) /* strictest alignment we want */

struct sockaddr_storage {
    uint8_t     ss_len;                   /* BSD: length of struct (optional) */
    sa_family_t ss_family;                /* address family (AF_INET, AF_INET6, ...) */

    /* Force alignment: a member with the largest alignment requirement. */
    long long   _ss_align;                /* ensures proper alignment on 64-bit */

    /* Padding to make the whole struct _SS_SIZE bytes long.
       The exact size here depends on sizeof(ss_len)+sizeof(ss_family)+sizeof(_ss_align). */
    char        _ss_pad[_SS_SIZE
                        - sizeof(uint8_t)
                        - sizeof(sa_family_t)
                        - sizeof(long long)];
};


accept() example:

struct sockaddr_storage client;        // Large generic container for ANY socket address (IPv4, IPv6, etc.)
socklen_t len = sizeof(client);        // Must tell accept() how much space we provide

// accept() fills 'client' with the peer's address and returns a new connected socket FD
int connfd = accept(listenfd, (struct sockaddr *)&client, &len);
// NOTE: cast to (struct sockaddr*) is required because accept() uses old POSIX API

// Determine what kind of address the client has (IPv4 or IPv6)
if (client.ss_family == AF_INET) {

    // Safe to reinterpret 'client' as sockaddr_in because ss_family says it's IPv4
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)&client;

    // Now you can access:
    // ipv4->sin_addr      (32-bit IPv4 address in network byte order)
    // ipv4->sin_port      (16-bit port in network byte order)
    // Convert to readable forms using inet_ntop(), ntohs(), etc.
}
else if (client.ss_family == AF_INET6) {

    // Safe to treat 'client' as IPv6 address
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&client;

    // Now you can access:
    // ipv6->sin6_addr     (128-bit IPv6 address)
    // ipv6->sin6_port     (port)
    // ipv6->sin6_scope_id (interface index for link-local addresses)
}








when process sends an address structure to the kernel

functions which send address structure to the kernel like: process -> kernel

bind()
connect()
sendto()


in this case kernel needs to know WHERE the structure is (pointer) HOW MANY BYTES to copy from user space (size value)
so we craft a struct and make kernel copy bytes in that struct that way.



when the kernel returns an address structure to the process

functions like: kernel → process.

accept()

recvfrom()

getsockname()

getpeername()

These return address information (peer address, local address, sender address, etc).

You tell the kernel how much memory space you are giving it (so it doesn't overflow).
The kernel tells you how many bytes it actually wrote into the structure.

“Here is a buffer with X bytes available — don’t overflow.”
“I filled Y bytes (Y may be less than or equal to X).”



Process -> kernel example:

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    // Create IPv4 TCP socket

    struct sockaddr_in serv;      
    // Create IPv4 socket address structure

    memset(&serv, 0, sizeof(serv));    
    // Always clear the structure (good practice)

    serv.sin_family = AF_INET;        
    // AF_INET = IPv4

    serv.sin_port = htons(8080);      
    // Port in NETWORK byte order

    inet_pton(AF_INET, "192.168.1.10", &serv.sin_addr);
    // Convert text → binary IPv4 and store it

    connect(sockfd, (struct sockaddr *)&serv, sizeof(serv));
    // process -> kernel:
    // 1) pass pointer to serv
    // 2) pass size (value) sizeof(serv)
    // kernel reads exactly sizeof(serv)

    close(sockfd);
}



Kernel -> Process example:

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int listenfd = socket(AF_INET6, SOCK_STREAM, 0);
    // Create IPv6 TCP socket

    struct sockaddr_in6 serv;
    memset(&serv, 0, sizeof(serv));

    serv.sin6_family = AF_INET6;
    serv.sin6_port = htons(8080);
    serv.sin6_addr = in6addr_any;

    bind(listenfd, (struct sockaddr *)&serv, sizeof(serv));
    listen(listenfd, 5);

    struct sockaddr_storage client;  
    // Large generic container for ANY address type

    socklen_t len = sizeof(client);
    // VALUE: "kernel, here is the max size you may write"

    int connfd = accept(listenfd, (struct sockaddr *)&client, &len);
    // kernel -> process:
    // kernel fills 'client' with peer address
    // kernel writes ACTUAL size into len (RESULT)

    if (client.ss_family == AF_INET6) {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&client;

        char str[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &ipv6->sin6_addr, str, sizeof(str));
        printf("Client IPv6: %s\n", str);
    }

    close(connfd);
    close(listenfd);
}



Byte Order

IPv4, IPv6, Port -  those must be sent in big-endian.

THIS METHODS USED FOR PORTS, FOR IP s inet_pton IS USED.

little endian -> big endian (host->network)

uint16_t htons(uint16_t x);   // 16-bit
uint32_t htonl(uint32_t x);   // 32-bit

big endian -> little endian (network->host)

uint16_t ntohs(uint16_t x);   // 16-bit
uint32_t ntohl(uint32_t x);   // 32-bit

for example when passing the port number must be done like that:

serv.sin_port = htons(8080);





Byte Manipulation

sockets contain bytes as data not strings. so socket fields must be treated as bytes. 

examples:

socket field must be zeros at start because they are binary, zero the structure:

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main() {
    struct sockaddr_in serv;

    // Zero the structure
    memset(&serv, 0, sizeof(serv));

    serv.sin_family = AF_INET;
    serv.sin_port   = htons(8080);
    inet_pton(AF_INET, "192.168.1.100", &serv.sin_addr);

    printf("Server ready: %s:%d\n",
           "192.168.1.100", ntohs(serv.sin_port));
    return 0;
}




assemble raw UDP packet buffer:

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>

struct udp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
};

int main() {
    uint8_t packet[1500];
    memset(packet, 0, sizeof(packet));   // clear packet

    struct udp_header uh;
    uh.src_port = htons(12345);
    uh.dst_port = htons(80);
    uh.length   = htons(sizeof(uh) + 5);
    uh.checksum = 0;

    // copy header into packet buffer
    memcpy(packet, &uh, sizeof(uh));

    // add payload “HELLO”
    memcpy(packet + sizeof(uh), "HELLO", 5);

    printf("UDP packet built: %zu bytes\n", sizeof(uh) + 5);
}




extract fields from received packet IPv4 header + 4-byte f:

#include <stdint.h>
#include <stdio.h>
#include <string.h>

struct ipv4_header {
    uint8_t  version_ihl;
    uint8_t  tos;
    uint16_t length;
    uint16_t id;
    uint16_t flags_fragment;
    uint8_t  ttl;
    uint8_t  protocol;
    uint16_t checksum;
    uint32_t src;
    uint32_t dst;
};

int main() {
    uint8_t packet[64] = {0};

    // pretend this was captured from the network
    struct ipv4_header rx;
    memcpy(&rx, packet, sizeof(rx));

    printf("Source IP raw hex: %08X\n", ntohl(rx.src));
}






compare two IP/MAC :

#include <stdio.h>
#include <string.h>

int main() {
    uint8_t mac1[6] = {0x00,0x1A,0x2B,0x3C,0x4D,0x5E};
    uint8_t mac2[6] = {0x00,0x1A,0x2B,0x3C,0x4D,0x5E};

    if (memcmp(mac1, mac2, 6) == 0)
        printf("MAC addresses match!\n");
    else
        printf("Different MAC.\n");
}






Expanding payload size by 4 bytes:

#include <stdio.h>
#include <string.h>

int main() {
    uint8_t packet[100] = {0};
    size_t header_len = 20;
    size_t payload_len = 10;

    // packet contains header + payload

    // shift payload forward to insert 4-byte option
    memmove(packet + header_len + 4,
            packet + header_len,
            payload_len);

    // insert the option bytes
    memset(packet + header_len, 0xAA, 4);

    printf("Shift complete.\n");
    return 0;
}









inet_pton()

IP addr String -> Network-byte-order THIS CONVERTS STRING TO BYTES + CONVERTS IT TO BIG-ENDIAN ORDER, READY TO SEND. ONLY FOR IP s.

works both for IPv4 and IPv6 

int inet_pton(int af, const char *src, void *dst);


| Parameter | Meaning                                                                                                 |
| --------- | ------------------------------------------------------------------------------------------------------- |
| `af`      | Address family: `AF_INET` for IPv4, `AF_INET6` for IPv6                                                 |
| `src`     | C string containing IP address ("192.168.1.1" or "fe80::1")                                             |
| `dst`     | Pointer to memory where binary value is stored: `struct in_addr*` for IPv4, `struct in6_addr*` for IPv6 |



| Return | Meaning                              |
| ------ | ------------------------------------ |
| **1**  | Successful conversion                |
| **0**  | Invalid address string               |
| **-1** | Invalid address family (`errno` set) |



inet_ntop()

Network-byte-order -> IP addr String 

Supports IPv4 & IPv6.

const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);

| Parameter | Meaning                                                               |
| --------- | --------------------------------------------------------------------- |
| `af`      | Address family (`AF_INET` / `AF_INET6`)                               |
| `src`     | Pointer to binary address (`struct in_addr` or `struct in6_addr`)     |
| `dst`     | Destination buffer for ASCII string                                   |
| `size`    | Buffer size (`INET_ADDRSTRLEN` for IPv4, `INET6_ADDRSTRLEN` for IPv6) |


| Return   | Meaning                                        |
| -------- | ---------------------------------------------- |
| **dst**  | Success, returns pointer to destination buffer |
| **NULL** | Error (`errno` set)                            |







sock_ntop


inet_ntop has a problem, to extract ip from socket structure you need to know where exactly the ip is in socket structure. so extraction code becames protocol dependent. separate exctraction methods needed for different protocols. so sock_ntop is a solution of it.  it extracts like this IP:PORT protocol does not metter.

Takes a generic struct sockaddr * pointer.

Checks the sa_family field.

Calls the appropriate function (inet_ntop) internally.

Adds the port number (if it’s nonzero) to the string.

Returns a presentation-format string like:

192.168.1.10:8080
[fe80::1]:8080














readn()

Reads exactly n bytes, looping until either all bytes are read or an error/EOF occurs.

Loops until all bytes are read.

Handles EINTR (interrupted system calls).

Returns bytes read, or -1 on error.



writen()

Writes exactly n bytes, looping if the kernel only writes a partial amount.

Similar logic to readn.

Ensures all bytes are written even if kernel buffer only accepts part.

Handles interruptions.





Buffered readline()

Uses an internal buffer read_buf to reduce system calls:

Reads a block of data at a time.

Returns bytes one by one to the caller.

Exposes internal buffer via readlinebuf(), so you can check leftover bytes.

Faster than the naive one-byte-per-read version.















✅ SOL_SOCKET (Generic Options)

✔ Applies to ALL socket types (TCP/UDP/raw)
✔ Level: SOL_SOCKET

1. SO_REUSEADDR

Type: int • get/set
Real-life scenarios:

Restarting a server without waiting for TIME_WAIT

Development (frequent restarts)

LAN or WAN (doesn't matter)

Example

int yes = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

2. SO_REUSEPORT

Type: int • get/set
Real-life scenarios:

Multi-worker servers (NGINX, DNS, QUIC)

High-performance per-core load balancing

Used in LAN + WAN

Example

int yes = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes));

3. SO_KEEPALIVE

Type: int • get/set
Real-life scenarios:

Detect dead TCP connections (SSH, MQTT, mobile apps)

Connections over unreliable WiFi/LAN

Server-client long-lived connections

4. SO_BROADCAST

Type: int • get/set
Real-life scenarios:

LAN only (broadcast does not cross routers)

Device discovery (printers, smart TVs, IoT)

Game LAN lobby discovery

DHCP client broadcasts

Example

int yes = 1;
setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));

5. SO_DEBUG

Type: int • get/set
Real-life scenarios:

Very rarely used, legacy debugging

Kernel tracing of socket activity (developer diagnostics only)

6. SO_DONTROUTE

Type: int • get/set
Real-life scenarios:

Send packets only to directly connected LAN, bypass routing

Used for debugging routing issues

Rare in production

7. SO_ERROR

Type: int • get only
Real-life scenarios:

Check asynchronous connect() completion

Detect connection failure states

Socket-level troubleshooting

Example

int err;
socklen_t len = sizeof(err);
getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &len);

8. SO_LINGER

Type: struct linger • get/set

Real-life scenarios:

Force TCP immediate RST to quickly close

Ensure all data is delivered before close()

Useful for financial trading systems, VoIP, strict protocols

Example

struct linger ln = {1, 5}; // wait 5 sec
setsockopt(fd, SOL_SOCKET, SO_LINGER, &ln, sizeof(ln));

9. SO_OOBINLINE

Type: int • get/set
Real-life scenarios:

Legacy TCP out-of-band data (rare)

Not used in modern systems

10. SO_RCVBUF / SO_SNDBUF

Type: int • get/set

Real-life scenarios:

Increase buffers for high-throughput transfers (VPN, FTP)

Decrease for low-latency apps (games, VoIP)

Common in both LAN and WAN

11. SO_RCVLOWAT / SO_SNDLOWAT

Type: int • get/set
Real-life scenarios:

Rarely used

Control minimum bytes before recv() returns

Used for very specific real-time protocols

12. SO_RCVTIMEO / SO_SNDTIMEO

Type: struct timeval • get/set

Real-life scenarios:

recv() timeout for UDP-based protocols

Real-time systems with strict deadlines

Avoid blocking indefinitely

13. SO_TYPE

Type: int • get only

Real-life scenarios:

Libraries verifying socket type (TCP/UDP)

Debugging

Never needed in normal applications

14. SO_USELOOPBACK

Type: int • get/set

Real-life scenarios:

Rare, used for routing sockets

Developer/testing only

✅ IPPROTO_IP (IPv4 Options)

Level: IPPROTO_IP

1. IP_TTL

Type: int • get/set
Real-life scenarios:

Limit reachability to LAN (TTL=1)

Security: stop packets from leaving network

Debugging routes

2. IP_TOS (DSCP/TOS/QoS)

Type: int • get/set

Real-life scenarios:

VoIP priority

Gaming priority

Enterprise QoS routing

Works on LAN + WAN (if routers support QoS)

3. IP_HDRINCL

Type: int • get/set
Real-life scenarios:

Raw packet generation (nmap, traceroute, ping)

Security tools

Packet crafting

Advanced use only

4. IP_OPTIONS

Type: variable buffer • get/set
Real-life scenarios:

Very rare

Loose source routing (deprecated)

Debugging only

5. IP_RECVDSTADDR

Type: int • get/set
Real-life scenarios:

UDP servers listening on multiple IPs

Need destination IP of incoming packet

For multi-IP hosts on LAN/WAN

6. IP_RECVIF

Type: int • get/set

Real-life scenarios:

Know which interface a UDP packet arrived on

Important for routers, DHCP servers, and firewalls

7. Multicast options:
IP_MULTICAST_IF

Select outgoing interface for multicast.

Real-life:

IPTV

LAN video streaming

Routers with multiple NICs

IP_MULTICAST_TTL

Limit multicast range.

Real-life:

TTL=1 for LAN-only multicast

Used widely in IPTV and discovery

IP_MULTICAST_LOOP

Control whether multicast is echoed back.

Real-life:

Disable in servers to avoid duplicate data

Enable for local testing

IP_ADD_MEMBERSHIP / IP_DROP_MEMBERSHIP

Join/leave multicast groups.

Real-life:

IPTV

Stock price feeds

Multiplayer games

IoT

Almost always LAN

8. Source-specific multicast options

IP_BLOCK_SOURCE

IP_UNBLOCK_SOURCE

IP_ADD_SOURCE_MEMBERSHIP

IP_DROP_SOURCE_MEMBERSHIP

Real-life:

Advanced multicast filtering

ISPs, routers, IPTV headends

✅ IPPROTO_IPV6 (IPv6 Options)

Level: IPPROTO_IPV6

1. IPV6_V6ONLY

Type: int • get/set
Real-life:

Disable IPv4 compatibility

Required for dual-stack servers

Security hardening

2. IPV6_UNICAST_HOPS

IPv6 TTL equivalent.

3. IPV6_MULTICAST_HOPS

Multicast hop limit.

4. IPV6_JOIN_GROUP / IPV6_LEAVE_GROUP

Join/leave IPv6 multicast.

Real-life:

Router Advertisements

Multicast Listener Discovery (MLD)

IPv6-based IPTV

LAN heavy usage

5. IPV6_RECVPKTINFO

Get interface + destination info.

Real-life:

Multihomed IPv6 apps

Advanced routing

VPN tunneling

6. IPV6_DONTFRAG

Drop oversized packets.

Real-life:

PMTU testing

VPN tunnels needing fixed MTU

✅ IPPROTO_TCP (TCP Options)

Level: IPPROTO_TCP

1. TCP_NODELAY

Type: int • get/set
Disable Nagle.

Real-life:

Gaming

Financial trading

HTTP/1.1 pipelining

VoIP signaling

LAN or WAN

2. TCP_MAXSEG

Type: int • get/set
Control MSS.

Real-life:

VPN tunneling

Avoid fragmentation

Custom TCP stacks

3. Keepalive tuning

TCP_KEEPIDLE

TCP_KEEPINTVL

TCP_KEEPCNT

Real-life:

Detect dead TCP peers quickly

Mobile networks

Server keepalive policies

4. TCP_QUICKACK

Disable delayed ACKs temporarily.

Real-life:

Low-latency request/response

Database connections

LAN messaging servers

✅ IPPROTO_UDP (UDP Options)

Linux-specific performance options.

1. UDP_CORK

Batch UDP packets.

Real-life:

High-throughput video delivery

Reduce overhead for small packets

Streaming servers (LAN/WAN)

2. UDP_GRO

Enable hardware offload.

Real-life:

10Gb/40Gb/100Gb NICs

Packet capture systems

Datacenter LANs

✅ IPPROTO_SCTP (SCTP Options)

Used in telecom, 5G, Diameter, SS7, WebRTC (DataChannels).

1. SCTP_INITMSG

Set number of inbound/outbound SCTP streams.

Real-life:

Telecom signalling

Multi-streaming applications (VoIP backends)

2. SCTP_NODELAY

Disable bundling.

3. SCTP_AUTOCLOSE

Close after idle time.

Real-life:

Telecom switches needing idle cleanup

4. SCTP_EVENTS

Subscribe to SCTP event notifications.

Real-life:

Multihoming failover

Path failure events

Telecom-grade reliability

5. SCTP_PEER_ADDR_PARAMS

Control per-destination behavior.

Real-life:

Failover routing

Multi-homed servers

Telecom networks





























RAW SOCKETS


raw sockets enable read and write packets under TCP/UDP layer, packets of ICMP, ICMPv6, IGMP packets. normal sockets does not allow it.

raw sockets enable to directly send and receive:

ICMPv4

IGMPv4

ICMPv6




IPv4 header has protocol field which tells what protocol packet carries 

TCP = 6, UDP = 17, ICMP = 1, IGMP = 2

normal kernels only allow this protocols not others. but there are other protocols for example protocol = 89, (OSPF) so by writing program which uses raw sockets to read and send OSPF packets we now can use OSPF protocol, we could not it with normal kernel.




normally kernel handles building IP HEADER but by using raw sockets + IP_HDRINCL, we can craft our own IP HEADER with full byte control. so we can build custom IPv4 packets, UDP packets, TCP packets.

raw sockets also give ability to inspect full packet also its IP HEADER, not only the payload like its as default.

in 802.11 raw sockets used to craft custom deauth packets, auth req packets, evil twin, mac spoof, handshake capture, fragattacks, 


packets which can be crafted with RAW SOCKETS:

✅ A. ICMP (IPv4 + IPv6)

Raw sockets allow crafting:

ICMPv4

Echo Request / Reply (ping)

Time Exceeded

Destination Unreachable

Redirect

Timestamp Request/Reply

Router Solicitation/Advertisement

Source Quench (obsolete but still possible)

Parameter Problem

ICMPv6

Echo Request / Reply

Neighbor Solicitation / Advertisement

Router Solicitation / Advertisement

Redirect

Packet Too Big

Time Exceeded

Multicast Listener Discovery (MLD) messages

✅ B. IGMP (IPv4 Multicast Management)

Raw sockets allow crafting:

Membership Query

Membership Report (v1, v2, v3)

Leave Group

Source-specific multicast reports (IGMPv3)

✅ C. Any IPv4 or IPv6 Packet With a Custom Protocol Number

Raw sockets let you craft IP packets with any protocol number, for example:

Common examples

OSPF (Protocol 89)

GRE (47)

ESP (50) — IPsec

AH (51) — IPsec

SCTP (132)

PIM (Protocol 103)

VRRP (112)

EIGRP (88)

CARP (112, modified)

L2TP (115)

✔ basically any protocol listed in IANA’s Protocol Numbers registry.
✅ D. Custom IPv4 Headers with IP_HDRINCL

Using raw sockets + IP_HDRINCL, you can craft:

Custom IPv4 packets

Custom UDP packets (manually create UDP + IP header)

Custom TCP packets (manually create TCP + IP header)

This allows:

SYN flood test packets

Packets with fake source IPs (spoofing for research)

Custom TTL or fragmentation behavior

Packet injection for scanning or traceroute-style tools





how to create RAW SOCKET 

int sockfd;
sockfd = socket(AF_INET, SOCK_RAW, protocol);


AF_INET → IPv4

SOCK_RAW → creates a raw socket

protocol → IP protocol number (e.g., IPPROTO_ICMP, IPPROTO_IGMP, etc.)


only root can create RAW SOCKET 





IP_HDRINCL option - specifies who creates IPv4 header me or kernel.

const int on = 1;
setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on));


its on, must be handled by me.



bind()

Unlike TCP/UDP, bind() only sets the local IP address. There is no port for raw sockets. If you call bind → the source IP address is forced to what you specify. If you don’t call bind → the kernel automatically uses the primary address of the outgoing interface.



connect()

Again, raw sockets have no ports, so connect only sets the destination IP address. You can call write() or send() No need to supply destination address each time You don’t need sendto() because the foreign address is already known.

useful when:

pinging repeatedly

traceroute jumps

sending repeated OSPF packets

testing ICMP or custom protocol communication








RAW SOCKET OUTPUT

Raw socket output is mainly about how you craft packets.

raw socket output heavily depends on whether IP_HDRINCL is ON or OFF. because this tells who constructs the IP HEADER me or kernel.

raw socket output is sent same way as normal sockets.

sendto() → send with destination address

sendmsg() → advanced sending with control data

write(), writev(), or send() only if the socket is connected (because connect sets a fixed destination IP)









if IP_HDRINCL = OFF kernel builds IP HEADER means:

✔ Data passed to sendto() begins after the IP header

Kernel sets:

Source IP (unless bind was used)

Destination IP (from sendto or connect)

IPv4 header checksum

Identification field

Flags, Fragment offset (as needed)

TTL and TOS (unless modified by socket options)

Protocol field → matches 3rd argument to socket()



You only provide:

ICMP payload

IGMP payload

Custom protocol payload







if IP_HDRINCL = ON user builds IP HEADER

✔ Data passed to sendto() starts at the first byte of your IP header

You must construct:

Full IPv4 header

All header fields

All payload data

All upper-layer checksums (ICMP, UDP, TCP, etc.)

Kernel still helps with a few fields

The kernel will:

Set Identification field if you set it to 0

Always compute the IPv4 header checksum

Possibly handle IP options (depends on OS)

✔ You must include the size of the IP header in your write length

✔ Kernel still fragments packets that exceed the MTU

IPv4 checksum - calculated and set by kernel always.

but there are some fields, if value is set manually kernel will not touch it:

IPv4 Identification field (ip_id) - if non zero value set, kernel will not touch it.








in case of IPv6 there is no any equivalent for IP_HDRINCL

You cannot write a complete IPv6 header via a raw socket.

✔ If you absolutely must craft full IPv6 packets

→ must use datalink access (Chapter 29), not raw sockets.


























RAW SOCKET INPUT

To receive EVERY packet on a LAN (including: TCP, UDP, ARP, DHCP, IGMP, ICMP, STP, IPv6 ND), you must drop down to datalink layer.

The kernel gives a copy of the received packet to the raw socket’s receive queue.

kernel just gives me copyes of what received but still handles and process packets himself. so Raw sockets can ONLY observe packets. You cannot stop kernel handling with raw sockets. You must use datalink layer capture + dropping. 

If you want your program, not the kernel, to handle packets, you must use:

AF_PACKET + eBPF/XDP (Linux only)

receive full Ethernet frames

drop them before the kernel sees them

pass only selected packets to user space

let your C code handle everything manually

This is EXACTLY what firewalls, IDS/IPS, DPDK-based apps, and custom routers do.

How to drop packets before the kernel:

Attach an eBPF/XDP program to a NIC:

XDP_DROP         → kernel never sees the packet  
XDP_PASS         → allow kernel to process  
XDP_REDIRECT     → redirect to user mode or another interface


Your C program then handles the packets at L2 or L3.


i can drop them so kernel will not touch it then i will process them inspect them and send respond for them and kernel will not distract me.


The kernel does NOT pass every packet to raw sockets.

✔ ICMP, IGMP, unknown protocols → delivered

❌ TCP and UDP → NOT delivered

Because TCP and UDP have dedicated protocol handlers inside the kernel.




TCP/UDP

You cannot capture incoming TCP or UDP packets using raw sockets.

TCP/UDP belong to their own protocol handlers in the kernel (TCP stack, UDP stack).The kernel directly passes them to the TCP/UDP socket that owns them. TCP/UDP packets, you must read packets at the datalink layer



ICMP

Most ICMPv4 packets ARE passed to raw sockets, after the kernel handles them. 

BUT three ICMP messages are NOT passed:

Echo Request

Timestamp Request

Address Mask Request

kernel responds them automatically. 	



IGMP

All IGMP packets are passed to raw sockets after kernel processing.



UNKNOWN PROTOCOL

If the kernel sees an IPv4 packet with an unknown protocol number, it always delivers it to raw sockets.



Fragmented packets

If a datagram is fragmented:

raw socket gets nothing

until ALL fragments are reassembled

Only the final, reassembled packet is delivered.

Raw sockets operate at the IP layer




When does a raw socket receive the packet?

Every raw socket is checked. A packet is delivered only if ALL the following match:

✔️ Match 1: Protocol filter

If raw socket was created with:

socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);


Then the incoming packet must have:

IP header protocol = 1 (ICMP)

✔️ Match 2: Local (bind) filter

If your raw socket called:

bind(sockfd, "192.168.1.10", ...)


Then packet must have:

destination IP = 192.168.1.10

✔️ Match 3: Foreign (connect) filter

If raw socket called:

connect(sockfd, "8.8.8.8", ...)


Then incoming packet must have:

source IP = 8.8.8.8

🟡 If NO filters — socket gets everything

If:

protocol = 0 (wildcard)

no bind()

no connect()

Then the raw socket receives every raw IP packet allowed by kernel rules.



📦 What exactly is delivered to the application?

IPv4 raw sockets:

You get:

Entire IPv4 header

Entire payload

IPv6 raw sockets:

You do NOT receive the IPv6 header.

You only get:

the payload after IPv6 header + extension headers

This is a major design difference.

Why?

Because IPv6 APIs avoid exposing full headers except through ancillary data.




🧰 ICMPv6 Filtering

Since ICMPv6 handles:

Router solicitation/advertisement

Neighbor discovery (ARP equivalent)

Multicast listener discovery (IGMP equivalent)

Error messages

→ a raw ICMPv6 socket would receive a huge amount of traffic.

So IPv6 provides filters via struct icmp6_filter.

You can:

allow all

block all

allow specific types

block specific types

Example: only allow Router Advertisements:

struct icmp6_filter myfilt;
ICMP6_FILTER_SETBLOCKALL(&myfilt);
ICMP6_FILTER_SETPASS(ND_ROUTER_ADVERT, &myfilt);

setsockopt(fd, IPPROTO_ICMPV6, ICMP6_FILTER, &myfilt, sizeof(myfilt));















PING


PING uses raw sockets NOT DATALINK ITS L3

Ping needs to construct ICMP messages manually → this requires raw socket

Raw IP socket still gives you access to IP header + ICMP header

No need for Ethernet header



























DATALINK 

with datalink layer we can capture frames like: 


Ethernet II

[ dst MAC (6) ][ src MAC (6) ][ EtherType (2) ][ payload (46–1500) ][ FCS (4) ]

EtherType indicates what protocol is in payload (0x0800 = IPv4, 0x86DD = IPv6, 0x0806 = ARP, 0x8100 = 802.1Q VLAN tag, etc.).


ARP

Not an IP datagram, so raw IP sockets (AF_INET, SOCK_RAW, IPPROTO_*) will not receive ARP. You must capture at the datalink layer (BPF/pcap/AF_PACKET) to see ARP.


VLAN (IEEE 802.1Q)

[ dst MAC ][ src MAC ][ 0x8100 ][ VLAN tag (TPID/TCI) ][ EtherType ][ payload ]




PPP

Used for serial links, PPPoE, some VPN tunnels.


non-IP packets

outgoing packets before IP transmits them

incoming packets before IP handles them




You see every frame your interface receives — even ones not destined for you — if you put NIC into promiscuous mode.


you can see:

✔ Non-IP protocols

Examples:

ARP

RARP

STP

LLDP

custom protocols






you can buid:

ARP daemon

RARP server

DHCP server (though DHCP uses IP, still needs raw access sometimes)

Custom L2 authentication protocols

Ethernet-based discovery tools





Limitation: switched networks

On a switch, you only see:

Broadcast

Multicast you are subscribed to

Unicast frames only if addressed to you

To see all LAN traffic you need:

Port-mirroring / SPAN

A tap device

A hub (historical)

To see all traffic on a switched LAN:

Port mirroring / SPAN: configure the switch to copy all traffic from specified ports or VLANs to the monitoring port where your machine is attached.

Network TAP: hardware device that splits the fiber/copper to mirror traffic—reliable and passive.

ARP/Gratuitous ARP trickery (network abuse) can make switches flood, but not recommended.








libpcap

libpcap hides all this. Your program sees one API, and libpcap uses BPF or DLPI or SOCK_PACKET internally.









BPF

BPF hooks into a NIC driver:

on receive (right after packet arrives from wire)

on transmit (right before leaving NIC)

This is why BPF timestamps are extremely accurate.

On receive (RX): right after the NIC driver hands the frame to the kernel, BPF gets a copy to apply filtering and optionally buffer for user-space. Then kernel continues processing (IP stack, ARP handling, etc.).

On transmit (TX): just before a frame is handed to the NIC driver to be transmitted, BPF gets a copy (for timestamping/monitoring). Then driver sends it

If you want to stop kernel processing, XDP to drop packets at driver level.







BPF features




filtering

filtering packets before coping them to program:

udp or tcp
host 1.1.1.1
tcp and port 80
tcp[13:1] & 0x7 != 0       # SYN, FIN, RST


The ASCII filter is compiled into BPF bytecode using pcap_compile().





snaplen

You can request only the first N bytes of each packet.

Most sniffers only need headers.

Reduces kernel → user copy cost.

example:

snaplen = 96 bytes

Enough for Ethernet + IPv6 + TCP headers + a bit of payload.

tcpdump default is 96.





Double-buffering & read timeout

Each BPF instance has:

two kernel buffers

while one fills, the other is copied to user-space during read()

This avoids losing packets.


BPF issues a read() only when:

buffer is full, or

read timeout expires

tcpdump uses 1 second timeout.
RARP daemon uses 0 → immediate return.





Promiscuous mode

BPF can tell NIC to accept frames not addressed to you.

Example:

ioctl(fd, BIOCPROMISC, &on);


Used by sniffers like tcpdump.

promiscoud vs wifi monitor: Promiscuous on Wi-Fi (if supported) might accept frames for other MACs within the same association, but monitor gives you raw 802.11, including frames from devices the NIC is not associated with, and allows capture of management/control frames and frames on other channels if the card is set to them.










Writing packets (sending)

BPF is usually used for reading, but you can write arbitrary L2 frames.

Used for:

RARP server sending RARP replies (which are NOT IP packets)

custom protocols

Ethernet-based tools

If you want to send IP packets, you don’t need BPF — use a raw socket + IP_HDRINCL.











Why libpcap is introduced

Because:

BPF is only for BSD-like systems.

DLPI is only for SysV systems.

SOCK_PACKET was Linux-only and is now deprecated.

libpcap:

abstracts all these methods

gives you common API: pcap_open_live(), pcap_loop(), pcap_compile(), etc.

ensures your program is portable across Unix-like systems

Wireshark, tcpdump, Snort, Zeek all use libpcap.

















✔ Example 1 — tcpdump

Uses:

promiscuous mode

snaplen

filtering

BPF read buffers

timestamp accuracy









✔ Example 3 — constructing DNS packets manually

Author builds UDP packets manually using raw sockets (IP_HDRINCL).
Replies are captured using libpcap, not the kernel’s UDP stack.

This allows them to test whether DNS server generates UDP checksums or not.












snort:

How it works: uses rules (signatures) to match packet payloads/headers (e.g., “alert if TCP payload contains this pattern”).






Raw sockets vs AF_PACKET vs BPF vs XDP — quick guide

Raw IP socket (SOCK_RAW, AF_INET): lets you read/write IP datagrams and build your own IP header (sometimes limited). Works at L3 (IP). Good for ICMP, OSPF, custom IP protocols. Kernel still owns NIC.

AF_PACKET (packet socket): raw access to full L2 frames on Linux. You can read/write Ethernet frames in user space. Lower-level than raw IP sockets.

BPF / libpcap: kernel filter + buffer + timestamp. You open /dev/bpf (BSD) or use libpcap which abstracts BPF/DLPI/packet sockets. BPF gives a safe, efficient tap to user space.

XDP/eBPF: runs code at the driver level (very early) — can drop or redirect packets before the kernel IP stack sees them. Use this if you need to prevent kernel processing.

DPDK / netmap / PF_RING: user-space kernel-bypass frameworks for maximum performance and absolute control over RX/TX.






| Protocol       | Layer          | Seen by Raw IP Socket? | Seen by pcap? |
| -------------- | -------------- | ---------------------- | ------------- |
| **ARP**        | L2             | ❌ No                   | ✔ Yes         |
| **IPv6 NS/NA** | L3 inside IPv6 | ✔ Yes                  | ✔ Yes         |




Ethernet = 802.3

Wi-Fi = 802.11

VLAN = 802.1Q

Bridging = 802.1D








🔹 BPF / libpcap / AF_PACKET / monitor mode

These hook before the kernel’s IP layer, directly at the NIC level.

You see:

Ethernet frames

ARP (non-IP)

VLAN tagged frames

STP (non-IP)

LLDP (non-IP)

PPPoE

CDP, RARP, NetBIOS

Any random L2 protocol

Broken packets

Duplicates

Bad checksums (if NIC doesn’t drop)

→ TX/RX hooks here mean all packets.















Promiscuous mode = Ethernet feature

NIC accepts all valid Ethernet frames, but still only receives what the switch sends you.

Monitor mode = Wi-Fi feature

NIC receives radio frames, including:

management frames

control frames

encrypted data frames

retransmissions

corrupted frames

AP transmissions not meant for you

👉 Monitor mode is much deeper than promiscuous mode.

Promiscuous mode ≠ monitor mode.

















LINUX DATALINK ACCESS

PF_PACKET (Modern, Correct Method)

Introduced in Linux 2.2+ and used by:

tcpdump

Wireshark

Suricata

Snort

Zeek/Bro

libpcap

every L2 sniffer in existence

Two modes:
Mode	Description
SOCK_RAW	You get full L2 frame: Ethernet header + payload
SOCK_DGRAM	"Cooked" packets: Linux strips L2 header and gives cleaned metadata
Examples:
Receive everything on the NIC:
fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

Receive only IPv4 frames:
fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_IP));

Receive only ARP:
fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));















libpcap

libpcap is the portable, OS-independent abstraction layer for packet capture.
Instead of applications having to know BPF, DLPI, NIT, PF_PACKET, etc., they just use libpcap, and libpcap internally chooses the correct mechanism.



Application (tcpdump, Wireshark, custom tools)
           ↓
        libpcap (portable API)
           ↓
 OS-specific capture engine (BPF/DLPI/PF_PACKET/NIT/etc.)
           ↓
      Network Interface





Provides a unified API

Regardless of OS:

On BSD → uses BPF

On HP-UX / Solaris → uses DLPI

On SunOS → uses NIT

On Linux → uses SOCK_PACKET or PF_PACKET

On Windows → WinPcap/Npcap (port of libpcap)

Your program doesn’t need to know which one.




Handles filtering (via BPF bytecode)

libpcap compiles filter expressions such as:

"tcp and port 80"


into BPF bytecode, and then loads that into:

kernel BPF engine (BSD/Linux)







Provides packet capture functions

Common functions include:

pcap_open_live() — open an interface for capturing

pcap_compile() — compile a filter

pcap_setfilter() — apply the filter

pcap_loop() or pcap_next() — read packets

pcap_close() — cleanup

tcpdump uses exactly these functions.









- INJECTION -- INJECTOIN -- INJECTION -- INJECTOIN -- INJECTION -- INJECTOIN -- INJECTION -- INJECTOIN -- INJECTION -- INJECTOIN -- INJECTION -- INJECTOIN -  


libpcap


✔ 1. pcap_inject()
int pcap_inject(pcap_t *p, const void *buf, size_t size);


Sends raw bytes on the wire.

Returns number of bytes written or -1 on failure.

Faster than pcap_sendpacket() because it avoids extra copies.

Use case:

High-performance packet generators

IDS/IPS systems

ARP spoofing, DHCP crafting, custom L2 protocols

✔ 2. pcap_sendpacket()
int pcap_sendpacket(pcap_t *p, const u_char *buf, int size);


Does exactly the same thing as pcap_inject().

Always returns 0 on success, non-zero on error.

Lower-level, older API.

Some platforms only implemented this first.





✔ Differences between the two (short version)

| Feature      | pcap_inject()    | pcap_sendpacket()     |
| ------------ | ---------------- | --------------------- |
| Return value | bytes sent       | 0 / error             |
| Performance  | slightly faster  | slightly slower       |
| Age          | newer API        | older API             |
| Support      | widely supported | universally supported |


Most programs use pcap_sendpacket() for compatibility.





✔ Linux backend for packet injection

When you call these functions, libpcap uses PF_PACKET (SOCK_RAW) internally:

pcap_sendpacket()
       ↓
Linux PF_PACKET socket
       ↓
NIC hardware


This means:

Full L2 frame control (destination MAC, source MAC, EtherType)

You can send packets even with bogus headers

You can craft non-IP protocols (ARP, PPPoE, STP, LLDP, your own protocols)

You bypass the IP stack entirely

It is true raw injection, not “raw IP” like with AF_INET raw sockets.






✔ Examples of what you can send
1. Custom Ethernet frame

Destination MAC: FF:FF:FF:FF:FF:FF

Source MAC: anything

EtherType: 0x1234

Payload: 200 bytes of your choosing

2. ARP spoofing

Crafted ARP reply packets.

3. DHCP discovery or ACKs
4. TCP packets with invalid flags or checksum (for research)
5. Entire custom protocols (Layer 2 or Layer 3)




✔ Requirements
root privileges or equivalent capabilities:
CAP_NET_RAW
CAP_NET_ADMIN


Newer Linux distributions allow non-root users to send raw packets if:

Network namespace grants permissions

Capabilities are assigned

The interface is configured appropriately




2. WinPcap / Npcap — ✔ Yes, true low-level packet injection

Windows supports full L2 injection only through Npcap (or legacy WinPcap):

✔ You can send:

Custom MAC addresses

Custom IP source addresses

Custom EtherType (ARP, IPv4, IPv6, LLDP, STP, custom protocol)

Entire Ethernet frames built manually as a byte array

Jumbo frames (if NIC supports)

✔ You bypass Windows TCP/IP stack completely.
✔ You are basically writing bytes directly to the NIC transmit ring.

Example in C (Npcap / WinPcap API):

pcap_sendpacket(handle, frame, frame_len);


Where frame is a manually crafted Ethernet frame, e.g.:

[Dst MAC][Src MAC][EtherType][Payload][Checksum]






🟣 Does Windows verify frame contents before sending?

No.

Npcap writes directly into the NIC driver API:

No IP checksum verification

No TCP checksum verification

No filtering

No IP layer involvement

Optional: you can even include your own FCS (NIC usually recalculates unless offload disabled)






libnet

libnet makes it easier to create:

Raw IP packets

Custom TCP/UDP packets

ARP, Ethernet, VLAN frames

ICMP, IGMP

Custom protocols

Full L2-crafted frames, bit-by-bit



💡 Why libnet? (Problem it solves)

Building packets manually with raw sockets or PF_PACKET is tedious:

You must:

fill every field in IP, TCP, UDP headers

compute checksums

calculate lengths

assemble data in proper order

handle byte ordering

handle fragmentation rules

handle link-layer headers when needed

libnet hides all that.





🔧 libnet provides two injection modes
1️⃣ Raw socket mode (L3 injection)

libnet builds the packet like this:

[IP Header][Protocol Header][Payload]


Then sends via a raw socket:

socket(AF_INET, SOCK_RAW, IPPROTO_RAW)


Kernel adds:

Ethernet header

Some automatic fields (depending on IP_HDRINCL)

Used for crafting:

IP

ICMP

TCP

UDP

2️⃣ Datalink mode (L2 injection)

libnet builds the entire Ethernet frame:

[Dst MAC][Src MAC][EtherType][Payload]


This is equivalent to Linux PF_PACKET/SOCK_RAW or pcap_sendpacket.

Used for crafting:

ARP

Ethernet frames

VLAN (802.1Q)

STP, LLDP

Custom protocols

Custom MAC-level attacks (MITM, ARP spoofing, LAN scanning)





🎯 libnet gives portable packet injection

raw sockets and PF_PACKET are not portable:

Linux has PF_PACKET

BSD has BPF

Solaris has DLPI

Windows has WinPcap/Npcap

AIX/HP-UX have their own APIs

Some systems restrict raw sockets

Some handle checksums differently

libnet abstracts all of that.

Your C code stays the same → libnet handles the platform differences.





🧱 libnet API structure (simple version)
Step 1 — Initialize the library
libnet_t *lnet = libnet_init(LIBNET_RAW4, device, errbuf);

Step 2 — Build protocol headers

e.g., build IP header:

libnet_build_ipv4(
    len, tos, id, frag, ttl, proto, checksum,
    src_ip, dst_ip, payload, payload_s, lnet, 0
);


Or build full Ethernet frame:

libnet_build_ethernet(
    dst_mac, src_mac, ETHERTYPE_IP, payload, payload_s, lnet, 0
);

Step 3 — Inject packet
libnet_write(lnet);

Step 4 — Cleanup
libnet_destroy(lnet);









Real-World Uses

libpcap is used by:

tcpdump

Wireshark (via WinPcap/Npcap)

Snort IDS / Suricata

Bro/Zeek

many custom security tools

Almost every network analysis tool relies on libpcap.








libpcap vs libnet

libpcap

libpcap is fundamentally a capture/sniffing library.

libnet gives separate templates for l2 and l3 packet crafting. libpcap does not help crafting packets. just sends the bytes you provide him.

libpcap does NOT:

build Ethernet headers

build IP/TCP/UDP headers

compute checksums

manage fragmentation

auto‑fill MAC or IP

support header stacking

support protocol construction






libnet for crafting and injection.


constructs Ethernet headers

constructs IP, TCP, UDP, ICMP, IGMP headers

manages stacking (L2→L3→L4)

computes checksums

computes lengths

handles source IPs/MACs

provides RAW and DATALINK injection modes

is portable across OSes


has direct functions for crafting specific packets:

libnet_build_icmp_echo(...);
libnet_build_ipv4(...);
libnet_build_ethernet(...);
libnet_write();




| Feature               | **libpcap**          | **libnet**                                 |
| --------------------- | -------------------- | ------------------------------------------ |
| Primary purpose       | Capture              | Craft + Inject                             |
| Inject support        | Basic                | Full packet injection stack                |
| Build Ethernet header | ❌ No                 | ✔ Yes                                      |
| Build IP header       | ❌ No                 | ✔ Yes                                      |
| Build UDP/TCP         | ❌ No                 | ✔ Yes                                      |
| Checksums             | ❌ You must calculate | ✔ Auto generated                           |
| L2 injection          | ✔ Yes                | ✔ Yes                                      |
| L3 injection          | ❌ No                 | ✔ Yes                                      |
| Header stacking       | ❌ No                 | ✔ Yes                                      |
| OS portability        | Very good            | Very good                                  |
| API difficulty        | Easy                 | Moderate                                   |
| Used by modern tools  | Capture tools        | Packet generators, scanners, ARP poisoners |







🛠 Which should you use for packet injection?
Use libnet if you want:

✔ L2 crafting (Ethernet, ARP, VLAN, STP…)
✔ L3 crafting (IP, ICMP, TCP, UDP)
✔ auto checksums
✔ header stacking
✔ simplest injection API
✔ portability
✔ safe built‑in structures

Use libpcap only if:

✔ You already have the full frame built manually
✔ You want to send raw bytes
✔ You do not need header construction
✔ You do not need checksums

RAW conclusion:

👉 libpcap is only a sender, libnet is a packet builder + sender.

This is why professional packet‑generation tools use libnet or PF_PACKET instead of libpcap.





🧰 What modern tools use which library?
Tools using libpcap (capture):

tcpdump

Wireshark

Snort (capture engine)

Suricata (capture on non-AF_PACKET)

Zeek/Bro

Nmap (for sniffing only)

Tools using libnet (injection):

Hping3 → uses libnet to build custom TCP/IP packets

Nemesis → classic packet generator, built on libnet

dsniff/arpspoof → uses libnet for ARP poisoning

yersinia → L2 attack tool

Many custom academic/security research tools

Tools using PF_PACKET directly (Linux only):

Scapy (Python raw crafting framework)

Suricata (modern)

Soft‑switches and IDS/IPS engines

Modern packet injection exploits/tools




✔ A raw socket cannot capture incoming UDP/TCP

Raw sockets only send full IP packets; they do not receive incoming transport packets in most systems.

Therefore, libpcap is necessary to read the raw layer-2 frames, including checksums




