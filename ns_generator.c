/* ns_generator.c
 *
 * Build an IPv6 Neighbor Solicitation (ICMPv6 type 135) with Source Link-Layer Address option,
 * compute ICMPv6 checksum, and simulate send (Ethernet frame).
 *
 * Compile:
 *   gcc ns_generator.c -o ns_generator
 * Run:
 *   ./ns_generator
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>

struct eth_hdr {
    uint8_t dst[6];
    uint8_t src[6];
    uint16_t ethertype;
} __attribute__((packed));

/* simple checksum helper (16-bit ones' complement) */
static uint16_t csum16(const void *buf, size_t len) {
    const uint8_t *d = buf;
    uint32_t sum = 0;
    while (len > 1) {
        sum += (d[0] << 8) | d[1];
        d += 2;
        len -= 2;
    }
    if (len) sum += d[0] << 8;
    while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);
    return (uint16_t)~sum;
}

/* ICMPv6 checksum over pseudo-header + icmp payload */
static uint16_t icmpv6_checksum(const struct ip6_hdr *ip6, const struct icmp6_hdr *icmp6, size_t icmp_len) {
    uint8_t pseudo[40 + 8]; /* src(16)+dst(16)+plen(4)+zeros(3)+nxt(1) */
    memset(pseudo,0,sizeof(pseudo));
    memcpy(pseudo, &ip6->ip6_src, 16);
    memcpy(pseudo+16, &ip6->ip6_dst, 16);
    uint32_t plen = htonl(icmp_len);
    memcpy(pseudo+32, &plen, 4);
    pseudo[39] = IPPROTO_ICMPV6;
    uint32_t sum = 0;
    sum += csum16(pseudo, 40);
    sum += csum16(icmp6, icmp_len);
    while (sum >> 16) sum = (sum & 0xffff) + (sum >> 16);
    return (uint16_t)~sum;
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
    /* Example values:
     * Our MAC: 02:11:22:33:44:55 (source)
     * Solicited-node multicast dst MAC for target fe80::1a2b:3c4d:5e6f:1111 is 33:33:ff:5e:6f:11
     * Source IP: fe80::211:22ff:fe33:4455
     * Target IP: fe80::1a2b:3c4d:5e6f:1111
     */

    uint8_t our_mac[6] = {0x02,0x11,0x22,0x33,0x44,0x55};
    uint8_t dst_mac[6] = {0x33,0x33,0xff,0x5e,0x6f,0x11};

    struct in6_addr src_ip, dst_ip, target_ip;
    inet_pton(AF_INET6, "fe80::211:22ff:fe33:4455", &src_ip);
    inet_pton(AF_INET6, "ff02::1:ff5e:6f11", &dst_ip); /* solicited-node multicast */
    inet_pton(AF_INET6, "fe80::1a2b:3c4d:5e6f:1111", &target_ip);

    uint8_t frame[1500];
    size_t off = 0;

    /* Ethernet header */
    struct eth_hdr eth;
    memcpy(eth.dst, dst_mac, 6);
    memcpy(eth.src, our_mac, 6);
    eth.ethertype = htons(0x86dd); /* IPv6 */
    memcpy(frame + off, &eth, sizeof(eth)); off += sizeof(eth);

    /* IPv6 header */
    struct ip6_hdr ip6;
    memset(&ip6,0,sizeof(ip6));
    ip6.ip6_flow = htonl(6<<28);
    ip6.ip6_plen = 0; /* fill later */
    ip6.ip6_nxt = IPPROTO_ICMPV6;
    ip6.ip6_hlim = 255;
    ip6.ip6_src = src_ip;
    ip6.ip6_dst = dst_ip;
    memcpy(frame + off, &ip6, sizeof(ip6)); off += sizeof(ip6);

    /* ICMPv6 Neighbor Solicitation: type 135 */
    struct {
        struct icmp6_hdr icmp;
        uint32_t reserved;
        struct in6_addr target;
        uint8_t options[8]; /* Source Link-Layer Addr option (type=1,len=1 + 6 bytes MAC) */
    } __attribute__((packed)) ns;

    memset(&ns,0,sizeof(ns));
    ns.icmp.icmp6_type = ND_NEIGHBOR_SOLICIT;
    ns.icmp.icmp6_code = 0;
    ns.reserved = 0;
    ns.target = target_ip;
    ns.options[0] = 1; /* type = Source Link-Layer Addr */
    ns.options[1] = 1; /* length in 8-octet units */
    memcpy(&ns.options[2], our_mac, 6);

    size_t icmp_len = sizeof(ns);
    /* set IPv6 payload length */
    uint16_t plen = htons((uint16_t)icmp_len);
    memcpy(((uint8_t*)&ip6)+4, &plen, 2); /* ip6_plen is at offset 4 in struct ip6_hdr in many systems */
    /* recopy ip6 to frame with updated plen */
    memcpy(frame + sizeof(struct eth_hdr), &ip6, sizeof(ip6));

    /* compute checksum */
    ns.icmp.icmp6_cksum = 0;
    ns.icmp.icmp6_cksum = icmpv6_checksum(&ip6, &ns.icmp, icmp_len);

    memcpy(frame + off, &ns, icmp_len); off += icmp_len;

    simulated_send(frame, off);
    return 0;
}
