/* arp_reply.c
 *
 * Build an ARP reply frame in memory and simulate send.
 * Compile:
 *   gcc arp_reply.c -o arp_reply
 * Run:
 *   ./arp_reply
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>

struct eth_hdr {
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t ethertype;
} __attribute__((packed));

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
    /* Example: we (responder) have IP 192.168.1.1 & MAC aa:bb:cc:dd:ee:ff
       original requester MAC is 44:8a:5b:12:34:56 and IP 192.168.1.20 */
    uint8_t our_mac[6] = {0xaa,0xbb,0xcc,0xdd,0xee,0xff};
    uint8_t req_mac[6] = {0x44,0x8a,0x5b,0x12,0x34,0x56};
    const char *our_ip = "192.168.1.1";
    const char *req_ip = "192.168.1.20";

    uint8_t frame[1500];
    size_t off = 0;

    struct eth_hdr eth;
    memcpy(eth.dst, req_mac, 6);
    memcpy(eth.src, our_mac, 6);
    eth.ethertype = htons(0x0806);
    memcpy(frame+off, &eth, sizeof(eth)); off += sizeof(eth);

    struct arp_pkt arp;
    arp.htype = htons(1);
    arp.ptype = htons(0x0800);
    arp.hlen = 6;
    arp.plen = 4;
    arp.oper = htons(2); /* ARP reply */
    memcpy(arp.sha, our_mac, 6);
    inet_pton(AF_INET, our_ip, arp.spa);
    memcpy(arp.tha, req_mac, 6);
    inet_pton(AF_INET, req_ip, arp.tpa);

    memcpy(frame+off, &arp, sizeof(arp)); off += sizeof(arp);

    simulated_send(frame, off);
    return 0;
}
