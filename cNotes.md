
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



<pre>

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


</pre>





-------------------------------------------------------------------------- Bitmasks ------------------------------------------------------------------------



|= → set a bit

&= ~ → clear a bit

^= → flip a bit

(tcp_flags >> i) & 1 → extract the i-th bit for printing


<pre>

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



</pre>






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






