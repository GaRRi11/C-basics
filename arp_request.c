/* arp_request.c
 *
 * Build a real ARP request (Ethernet frame + ARP payload) in memory,
 * then "send" it via a simulated send function that prints hex.
 *
 * Compile:
 *   gcc arp_request.c -o arp_request
 * Run:
 *   ./arp_request
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

/* Ethernet header */
struct eth_hdr {
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t ethertype;
} __attribute__((packed));

/* ARP packet (Ethernet + IPv4) */
struct arp_pkt {
    uint16_t htype;
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t oper;
    uint8_t sha[6];
    uint8_t spa[4];
    uint8_t tha[6];
    uint8_t tpa[4];
} __attribute__((packed));

/* Simulated send: print hex bytes */
void simulated_send(const uint8_t *buf, size_t len) {
    printf("Simulated send (%zu bytes):\n", len);
    for (size_t i=0;i<len;i++) {
        printf("%02x", buf[i]);
        if ((i+1)%16==0) printf("\n");
        else printf(" ");
    }
    if (len%16) printf("\n");
}

int main(void) {
    /* Example values: fill with something realistic */
    uint8_t my_mac[6]  = {0x44,0x8a,0x5b,0x12,0x34,0x56};  /* source MAC */
    uint8_t brd_mac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};  /* broadcast */
    const char *sender_ip_str = "192.168.1.20";
    const char *target_ip_str = "192.168.1.1";

    uint8_t frame[1500];
    size_t off = 0;

    /* Ethernet header */
    struct eth_hdr eth;
    memcpy(eth.dst, brd_mac, 6);
    memcpy(eth.src, my_mac, 6);
    eth.ethertype = htons(0x0806); /* ARP */
    memcpy(frame + off, &eth, sizeof(eth));
    off += sizeof(eth);

    /* ARP payload */
    struct arp_pkt arp;
    arp.htype = htons(1);         /* Ethernet */
    arp.ptype = htons(0x0800);    /* IPv4 */
    arp.hlen = 6;                 /* MAC len */
    arp.plen = 4;                 /* IPv4 len */
    arp.oper = htons(1);          /* ARP request */
    memcpy(arp.sha, my_mac, 6);
    inet_pton(AF_INET, sender_ip_str, arp.spa);
    /* target hardware unknown (zeros) */
    memset(arp.tha, 0, 6);
    inet_pton(AF_INET, target_ip_str, arp.tpa);

    memcpy(frame + off, &arp, sizeof(arp));
    off += sizeof(arp);

    /* Final frame length */
    simulated_send(frame, off);
    return 0;
}
