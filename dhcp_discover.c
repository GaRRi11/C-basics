/* dhcp_discover.c
 *
 * Build DHCPDISCOVER (Ethernet | IPv4 | UDP | BOOTP/DHCP) in-memory.
 * We compute IPv4 header checksum. For simplicity we set UDP checksum to 0 (allowed for IPv4).
 *
 * Compile:
 *   gcc dhcp_discover.c -o dhcp_discover
 * Run:
 *   ./dhcp_discover
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

struct eth_hdr {
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t ethertype;
} __attribute__((packed));

/* BOOTP/DHCP header (fixed part) */
struct bootp {
    uint8_t op;       /* 1=BOOTREQUEST */
    uint8_t htype;    /* 1 = Ethernet */
    uint8_t hlen;     /* hardware addr len = 6 */
    uint8_t hops;
    uint32_t xid;
    uint16_t secs;
    uint16_t flags;
    uint32_t ciaddr;
    uint32_t yiaddr;
    uint32_t siaddr;
    uint32_t giaddr;
    uint8_t chaddr[16];
    uint8_t sname[64];
    uint8_t file[128];
    /* options follow */
} __attribute__((packed));

/* IPv4 checksum */
static uint16_t ip_checksum(void *vdata, size_t length) {
    char *data = (char*)vdata;
    uint64_t acc=0;
    for (size_t i=0;i+1<length;i+=2) {
        acc += (uint8_t)data[i] << 8 | (uint8_t)data[i+1];
    }
    if (length&1) acc += (uint8_t)data[length-1] << 8;
    while (acc>>16) acc = (acc & 0xffff) + (acc>>16);
    return (uint16_t)(~acc);
}

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
    /* Example: send DHCPDISCOVER from client MAC 02:11:22:33:44:55 */
    uint8_t client_mac[6] = {0x02,0x11,0x22,0x33,0x44,0x55};
    uint8_t bcast_mac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    uint8_t frame[2048];
    size_t off = 0;

    /* Ethernet header */
    struct eth_hdr eth;
    memcpy(eth.dst, bcast_mac, 6);
    memcpy(eth.src, client_mac, 6);
    eth.ethertype = htons(0x0800); /* IPv4 */
    memcpy(frame + off, &eth, sizeof(eth)); off += sizeof(eth);

    /* IPv4 header (we will set src=0.0.0.0, dst=255.255.255.255) */
    struct iphdr ip;
    memset(&ip,0,sizeof(ip));
    ip.ihl = 5;
    ip.version = 4;
    ip.tos = 0;
    /* total length fill later */
    ip.id = htons(0x1234);
    ip.frag_off = 0;
    ip.ttl = 64;
    ip.protocol = IPPROTO_UDP;
    ip.saddr = inet_addr("0.0.0.0");
    ip.daddr = inet_addr("255.255.255.255");
    /* copy ip temporarily (checksum later after udp + bootp length known) */
    memcpy(frame + off, &ip, sizeof(ip)); off += sizeof(ip);

    /* UDP header */
    struct udphdr udp;
    udp.source = htons(68); /* client port */
    udp.dest = htons(67);   /* server port */
    udp.check = 0; /* we'll set to 0 (optional for IPv4) */

    /* BOOTP/DHCP payload */
    struct bootp b;
    memset(&b,0,sizeof(b));
    b.op = 1; b.htype = 1; b.hlen = 6; b.hops = 0;
    b.xid = htonl(0x3903F326); /* example xid */
    b.secs = 0; b.flags = htons(0x8000); /* broadcast flag */
    b.ciaddr = 0; b.yiaddr = 0; b.siaddr = 0; b.giaddr = 0;
    memcpy(b.chaddr, client_mac, 6);

    /* options: magic cookie + DHCPDISCOVER (53=discover), parameter request list and end */
    uint8_t options[312];
    size_t optoff = 0;
    /* magic cookie */
    options[optoff++] = 99; options[optoff++] = 130; options[optoff++] = 83; options[optoff++] = 99;
    /* DHCP Message Type (53) */
    options[optoff++] = 53; options[optoff++] = 1; options[optoff++] = 1; /* 1 = DHCPDISCOVER */
    /* Parameter Request List (55) ask for subnet, router, dns */
    options[optoff++] = 55; options[optoff++] = 3; options[optoff++] = 1; options[optoff++] = 3; options[optoff++] = 6;
    /* End (255) */
    options[optoff++] = 255;

    /* assemble UDP + BOOTP */
    size_t udp_payload_len = sizeof(b) + optoff;
    udp.len = htons((uint16_t)(8 + udp_payload_len)); /* UDP header(8) + payload */
    /* copy UDP header */
    memcpy(frame + off, &udp, sizeof(udp)); off += sizeof(udp);

    /* copy BOOTP */
    memcpy(frame + off, &b, sizeof(b)); off += sizeof(b);
    memcpy(frame + off, options, optoff); off += optoff;

    /* now set IP total length and checksum */
    size_t ip_total = sizeof(struct iphdr) + sizeof(struct udphdr) + udp_payload_len;
    ((struct iphdr*)(frame + sizeof(struct eth_hdr)))->tot_len = htons((uint16_t)ip_total);
    ((struct iphdr*)(frame + sizeof(struct eth_hdr)))->check = 0;
    /* compute ip checksum */
    uint16_t ipch = ip_checksum((uint8_t*)frame + sizeof(struct eth_hdr), sizeof(struct iphdr));
    ((struct iphdr*)(frame + sizeof(struct eth_hdr)))->check = ipch;

    simulated_send(frame, off);
    return 0;
}
