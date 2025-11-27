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
