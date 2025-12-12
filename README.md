
----------------------------------------------------------------------- POINTER ----------------------------------------------------------------------------


a pointer variable (*p = &k) always stores address. if we want to know the value which is stores in that certain address, we have to use dereference operator * so *p returns the value saved on that address. just p returns just memory address.


a normal variable (k = 5) stores value. acts as normal variable, when called prints out the value.


a value stored in pointer is the another variables memory address. 

dereferencing *p = getting value inside that address, the value that address is holding.


    int k = 5;

    int *p = &k;


print(k) - prints 5, the value of k.

print(&k) - prints 473288048, memory address of k.

print(p) - prints 473288048, the value of p, memory address of k.

print(*p) - prints 5, dereferences p so gives the value which the memory address holds. 

print(&p) - prints 21341311, memory address of p.









    int *arr = calloc(5 * sizeof(int));


calloc(5 * sizeof(int)) - allocated memory size of normal int. sets it value to 0 as default. returns it value to *arr. so

print(*arr) - prints 0;

print(arr) - prints the address of memory malloc allocated. 

if we do 

*arr = 10;

then

print(*arr) - prints 10;



The pointer's type tells C how many bytes to read. thats why pointer type must always match the type of variable its pointed to. 

int - reads 8 byte.

uint8_t reads 1 byte.


Adding 1 to a pointer moves by the size of the pointed type.

uint32_t *p;

p + 1; // moves 4 bytes forward

uint8_t *b;

b + 1; // moves 1 byte forward



----------------------------------------------------------------------- STRING ----------------------------------------------------------------------------






"Alice" → STRING literal

char *name = "Alice"; -> valid

char name = "Alice";   -> ❌ INVALID

Because:

"Alice" is 6 chars

char name can only store 1 char

Address       Value
-------------------------
0x1000        'A'
0x1001        'l'
0x1002        'i'
0x1003        'c'
0x1004        'e'
0x1005        '\0'



so when doing char *name = "Alice";

name is the pointer of that char. is stores the first character - "A".

name = 0x1000
*name → 'A'
*(name + 1) → 'l'
*(name + 2) → 'i'

when printing out that string with printf("%s", name); it starts printing out characters one by one and stops when reaches \0



'a' → CHARACTER literal

char letter = 'a';   // ✔ OK



----------------------------------------------------------------------- STRUCT ----------------------------------------------------------------------------


Fields are laid out next to each other.

Padding bytes may be inserted between fields for alignment.

DEFINING:

1.without typedef


struct {
    char *brand;
    int model;
} 

struct vehicle object -> we have to always write "struct" keyword before struct name and object name.



2.with typedef


typedef struct vehicle{
    char *brand;
    int model;
} vehicle;

vehicle object -> no need of "struct" keyword at beginning when creating object. we just write name of struct and name of object.


REACHING:

vehicle mycar;
mycar.brand = "Ford";
mycar.model = 2007;


. is used to reach actual struct variables.

but if we create a pointer which points to struct. for example:

struct vehicle *ptr = &mycar

and we want to reach the brand variable of mycar throught ptr pointer. we must do:

ptr->brand = "subaru"

Structs can contain pointers to other structs

linked list:

struct node {
    int value;
    struct node *next;
};


if we want to make a object of a struct from c libraries, for example sockaddr_in object we just include its library #include <netinet/in.h>  and then create object of it like that:

    struct sockaddr_in object;

then do object. and IDE shows list of all variables of that struct.

struct sockaddr_in object;
object.sin_family = AF_INET;
object.sin_port = htons(80);
object.sin_addr.s_addr = inet_addr("192.168.0.1");

this way we have sockaddr_in object configured with our values.




-------------------------------------------------------------- Function arguments by reference -------------------------------------------------------------




this is about how argument is passed to function in c. so if we have a variable 

int n = 10;

and we want to pass it to function. if we pass it normally like this:

addone(n);


function will get copy of n instead of actual n. so the memory address of the actual variable n and the n we passed to function will be different.

void addone(int n) {
    n++;     // only changes the copy
}

this function gets copy of n as an argument and changes it value then exits. function did its job but real variable n has not changed.

if we want to make function receive the actual variable and change it we have do it like that.

function must be defined like that 

void addone(int *n) {
    (*n)++;
}


and argument must be passed like that

addone(&n);   // &n = address of n

now function will actually modify the real n variable which we defined previously and make it 6.

so function will get pointer to real n value, dereference it and modify.




------------------------------------------------------------------------ Dynamic allocation ----------------------------------------------------------------

normal:

    struct sockaddr_in client;

this allocated memory in stack 

but if we dynamically allocate memory, its being allocated in heap.






dynamic:

    struct sockaddr_in *clientMalloc = (struct sockaddr_in*) malloc(sizeof(struct sockaddr_in));

malloc(sizeof(struct sockaddr_in))

Allocates enough memory for one sockaddr_in on the heap

Returns a void* (generic pointer, “pointer to memory with unknown type”)

(struct sockaddr_in *)

Tells the compiler: “Hey, this void pointer is actually a pointer to struct sockaddr_in”

Why needed?

So that you can use -> to access struct members:

clientMalloc->sin_family = AF_INET;


malloc must always be returned to pointer, malloc itself returns address of memory, so it must be stored in pointer.

Size must be known → sizeof(type)

Access members via pointer → ->









free(myperson);

Releases the memory allocated by malloc

Does not delete myperson itself → it still holds the old address

After freeing, accessing myperson->name or myperson->age → undefined behavior (dangerous!)

Good practice: set pointer to NULL after free



free(myperson);
myperson = NULL;


| Feature                    | `struct sockaddr_in client` | `struct sockaddr_in *clientMalloc = malloc(...)` |
| -------------------------- | --------------------------- | ------------------------------------------------ |
| Memory location            | Stack                       | Heap                                             |
| Size known at compile time | Yes                         | Must specify                                     |
| Lifetime                   | Ends when function ends     | Stays until `free()`                             |
| Freeing                    | Automatic                   | Must call `free(clientMalloc)`                   |
| Access                     | `client.sin_family`         | `clientMalloc->sin_family`                       |




when to use dynamic allocation:
1.Unknown size at compile time

You don’t know n until runtime

Stack cannot handle variable-length arrays safely in all cases

2.Large number of objects

Stack is limited (few MB)

Heap can allocate large memory blocks safely

3.Data needs to persist beyond function scope

Stack memory is destroyed when function ends

Heap memory persists until you free it

4.Dynamic data structures

Linked lists, trees, queues, etc.

Each node is allocated on heap with malloc

Example: server storing connections dynamically






--------------------------------------------------------------------- Arrays and Pointers ------------------------------------------------------------------



define:

char vowels[] = {'A', 'E', 'I', 'O', 'U'};
char *pvowels = vowels;


iterate:

for (int i = 0; i < 5; i++)
    printf("%c ", *(pvowels + i));  // prints: A E I O U





----------------------------------------------------------------------- UNION ------------------------------------------------------------------------------

union is a one memory, like a one variable, in it can be multiple variables but they will be treated as one variable, if overwrited one, others will be overwritten to, unions value will be the largest variables value in it. in this case double.

size of union = size of its biggest member:

union Example {
    int   a;   // 4 bytes
    char  b;   // 1 byte
    double c;  // 8 bytes
};


Size of union = 8 bytes (size of largest member)


so that union can max be 8 bytes. so biggest value it can hold is a double value, but it can also hold int value and char value, we can assign and treat it without casting the pointer, example:




in case of struct, we would need to cast pointer everytime.


for example:


union IPv4 {
    uint32_t asInt;
    uint8_t  bytes[4];
};

This creates a single 4-byte memory block that can be viewed two ways:

4 bytes: bytes[0] bytes[1] bytes[2] bytes[3]

1 integer: asInt (32-bit)

both uint8)t byte[4] and asInt are same memory.

if we do:

    ip.bytes[0] = 192;
    ip.bytes[1] = 168;
    ip.bytes[2] = 1;
    ip.bytes[3] = 1;


then this 

    printf("%u.%u.%u.%u\n", ip.bytes[0], ip.bytes[1], ip.bytes[2], ip.bytes[3]);

will print out 192.168.1.1

“When reading the bytes as a uint32_t on a little-endian machine, the integer value appears reversed.” so when we read we see = 16885952.

now we convert it to network endian:

    uint32_t netOrder = htonl(ip.asInt);

and then print it out 

    printf("IPv4 integer: %u\n", netOrder);


prints out 

IPv4 integer: 3232235777


this is integer value of out string 192.168.1.1 ip. 

the value 3232235777 is a mathematical count of which ip is mine from the 4294967296 entire ipv4 pool.






-------------------------------------------------------------------- Function Pointers ---------------------------------------------------------------------



A function pointer is a variable that stores the address of a function.

a function pointer stores the address of code.

for example for this function:

void someFunction(int arg) { ... }

memory layout is:

0x1000: machine code of someFunction

so A function pointer simply stores that memory address:

void (*pf)(int) = &someFunction;

Now pf points to address 0x1000.



a function pointer we use to call the function it points to for example:

pf(5);     // jumps to address 0x1000 and executes the function

this calls somefunction with argument 5.





syntax:

this is how we declare the function pointer, its not assigned yet.

void (*pf)(int);


Read from inside out:

✔ 1. pf is a variable
✔ 2. (*pf) means pf is a pointer
✔ 3. return type is void
✔ 4. function takes (int)

So it means:

pf is a pointer to a function returning void and taking int.


    pf = someFunction; // assign

now its assigned and points to someFunction.


 pf(5);

this way we call it.













here is a full code example of defining 3 different metods which handle 3 different packets differently then defining one function pointer and creating array of that function pointer. and making each function pointer point to each function.


#include <stdio.h>      // for printf()
#include <stdint.h>     // for uint8_t, standard fixed-width integer types
#include <arpa/inet.h>  // commonly used for networking functions (htons, htonl), not strictly needed here

// ---------------------------
// 1. Define a function pointer type
// ---------------------------

// typedef allows us to create a new name for a type
// PacketHandler is now a type that is a pointer to a function
// that takes a pointer to uint8_t (byte array) and a size_t length, and returns void
typedef void (*PacketHandler)(uint8_t* data, size_t len);
// uint8_t* data = pointer to the raw packet bytes
// size_t len  = how many bytes are in the packet

// ---------------------------
// 2. Define example packet handler functions
// ---------------------------

void handleLogin(uint8_t* data, size_t len) {
    // This function handles a LOGIN packet
    // data points to the payload of the packet (here, username bytes)
    // len tells how many bytes are in the username
    printf("LOGIN packet: username length = %zu\n", len);
}

void handleMessage(uint8_t* data, size_t len) {
    // This function handles a MESSAGE packet
    // Here we just print the first byte as an example
    printf("MESSAGE packet: first byte = %u\n", data[0]);
}

void handlePing(uint8_t* data, size_t len) {
    // Handles a PING packet
    // No payload needed, we just acknowledge it
    printf("PING packet received!\n");
}

// ---------------------------
// 3. Create a table of packet handlers
// ---------------------------

// Array of 256 PacketHandler function pointers
// Each index corresponds to a possible packet type (0x00 to 0xFF)
// We will store pointers to the correct handler function for each type
PacketHandler handlers[256];

// ---------------------------
// 4. Initialize the handlers table
// ---------------------------

void initHandlers() {
    // Assign handler functions to specific packet types
    // 0x01 = LOGIN
    handlers[0x01] = handleLogin;

    // 0x02 = MESSAGE
    handlers[0x02] = handleMessage;

    // 0x03 = PING
    handlers[0x03] = handlePing;

    // Other entries remain NULL by default
}

// ---------------------------
// 5. Function to "receive" a packet
// ---------------------------

void receivePacket(uint8_t type, uint8_t* data, size_t len) {
    // Look up the handler function for this packet type
    PacketHandler h = handlers[type];

    if (h != NULL) {
        // If a handler exists, call it with the packet data and length
        h(data, len);  // syntax: function pointer call
    } else {
        // If no handler is registered, print unknown packet
        printf("Unknown packet type: %u\n", type);
    }
}

// ---------------------------
// 6. Main function
// ---------------------------

int main() {
    initHandlers();  // initialize the handler table

    // Example packet data arrays
    uint8_t loginData[] = { 'A', 'l', 'i' }; // LOGIN packet payload
    uint8_t msgData[]   = { 42 };            // MESSAGE packet payload
    uint8_t pingData[]  = { };               // PING packet has no payload

    // Simulate receiving packets
    receivePacket(0x01, loginData, 3); // LOGIN
    receivePacket(0x02, msgData, 1);   // MESSAGE
    receivePacket(0x03, pingData, 0);  // PING
    receivePacket(0xFF, pingData, 0);  // Unknown packet type

    return 0;  // exit
}







-------------------------------------------------------------------------- Bitmasks ------------------------------------------------------------------------



|= → set a bit

&= ~ → clear a bit

^= → flip a bit

(tcp_flags >> i) & 1 → extract the i-th bit for printing




#include <stdio.h>   // for printf()
#include <stdint.h>  // for uint8_t (8-bit unsigned integer)

// ---------------------------
// TCP flags positions (bit index)
// ---------------------------
#define FIN 0   // FIN flag is bit 0
#define SYN 1   // SYN flag is bit 1
#define RST 2   // RST flag is bit 2
#define PSH 3   // PSH flag is bit 3
#define ACK 4   // ACK flag is bit 4
#define URG 5   // URG flag is bit 5

int main() {
    uint8_t tcp_flags = 0; // initialize flags byte to 0 (all flags cleared)

    // ---------------------------
    // Set flags
    // ---------------------------
    tcp_flags |= 1 << SYN; // set SYN bit (bit 1) to 1 using OR and shift
    tcp_flags |= 1 << ACK; // set ACK bit (bit 4) to 1

    // ---------------------------
    // Check if ACK flag is set
    // ---------------------------
    // AND the tcp_flags with mask (1 << ACK). If result is non-zero, bit is set
    if (tcp_flags & (1 << ACK))
        printf("ACK is set!\n");  // prints because ACK was set above

    // ---------------------------
    // Clear SYN flag
    // ---------------------------
    // ~(1 << SYN) inverts mask: 11111101
    // AND with tcp_flags clears SYN bit while leaving others unchanged
    tcp_flags &= ~(1 << SYN);

    // ---------------------------
    // Flip (toggle) FIN flag
    // ---------------------------
    // XOR with mask (1 << FIN) toggles the FIN bit
    tcp_flags ^= 1 << FIN; 

    // ---------------------------
    // Print resulting flags in binary
    // ---------------------------
    printf("TCP flags: 0b");
    for(int i = 7; i >= 0; i--)  // iterate from most significant bit to least
        printf("%d", (tcp_flags >> i) & 1); // shift right i times and AND with 1 to get bit value
    printf("\n");  // newline

    return 0;  // exit program
}










----------------------------------------------------------------------------- FD ---------------------------------------------------------------------------


A file descriptor is just a small integer that the kernel uses to track open resources. Standard FDs when a process starts:

FD	Name	Description
0	STDIN	standard input
1	STDOUT	standard output
2	STDERR	standard error



These are automatically opened and managed by the kernel for every process.



write(int fd, const void *buf, size_t count) → writes count bytes from buf to the file/socket/resource identified by fd.

Example: fd = 1 → stdout (process outputs data).

read(int fd, void *buf, size_t count) → reads up to count bytes into buf from the file/socket/resource identified by fd.

Example: fd = 0 → stdin (process reads input).





------------------------------------------------------------------------- SOCKET ---------------------------------------------------------------------------


Creating a Socket

Template:

socket(domain, type, protocol)

Example:

int sockfd = socket(AF_INET, SOCK_STREAM, 0);



domain → AF_INET for IPv4, AF_INET6 for IPv6, AF_UNIX for local communication

type → SOCK_STREAM for TCP, SOCK_DGRAM for UDP












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




