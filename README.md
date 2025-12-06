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


