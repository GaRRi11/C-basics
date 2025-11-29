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
