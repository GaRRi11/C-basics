/* rs_ra.c
 *
 * Build an ICMPv6 Router Solicitation and a Router Advertisement (with a single
 * Prefix Information option). Both are constructed in-memory and printed (simulated send).
 *
 * Compile:
 *   gcc rs_ra.c -o rs_ra
 * Run:
 *   ./rs_ra
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

static uint16_t csum16(const void *buf, size_t len) {
    const uint8_t *d = buf; uint32_t sum=0;
    while (len>1) { sum += (d[0]<<8)|d[1]; d+=2; len-=2; }
    if (len) sum += d[0]<<8;
    while (sum>>16) sum = (sum & 0xffff) + (sum>>16);
    return (uint16_t)~sum;
}
static uint16_t icmpv6_checksum(const struct ip6_hdr *ip6, const struct icmp6_hdr *icmp6, size_t icmp_len) {
    uint8_t pseudo[40]; memset(pseudo,0,sizeof(pseudo));
    memcpy(pseudo, &ip6->ip6_src, 16);
    memcpy(pseudo+16, &ip6->ip6_dst, 16);
    uint32_t plen = htonl(icmp_len); memcpy(pseudo+32, &plen, 4);
    pseudo[39] = IPPROTO_ICMPV6;
    uint32_t sum = 0;
    sum += csum16(pseudo, 40);
    sum += csum16(icmp6, icmp_len);
    while (sum>>16) sum = (sum & 0xffff) + (sum>>16);
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

/* Build Router Solicitation (RS) */
void build_rs(uint8_t *frame, size_t *flen) {
    uint8_t our_mac[6] = {0x02,0x11,0x22,0x33,0x44,0x55};
    uint8_t dst_mac[6] = {0x33,0x33,0x00,0x00,0x00,0x02}; /* all-routers multicast ff02::2 -> eth 33:33:00:00:00:02 */
    struct in6_addr src_ip, dst_ip;
    inet_pton(AF_INET6, "fe80::211:22ff:fe33:4455", &src_ip);
    inet_pton(AF_INET6, "ff02::2", &dst_ip);

    size_t off=0;
    struct eth_hdr eth;
    memcpy(eth.dst, dst_mac, 6); memcpy(eth.src, our_mac, 6); eth.ethertype = htons(0x86dd);
    memcpy(frame+off, &eth, sizeof(eth)); off += sizeof(eth);

    struct ip6_hdr ip6; memset(&ip6,0,sizeof(ip6));
    ip6.ip6_flow = htonl(6<<28);
    ip6.ip6_nxt = IPPROTO_ICMPV6; ip6.ip6_hlim = 255;
    ip6.ip6_src = src_ip; ip6.ip6_dst = dst_ip;
    memcpy(frame+off, &ip6, sizeof(ip6)); off += sizeof(ip6);

    /* ICMPv6 RS: type 133, Code 0, Reserved 4 bytes, Option: Source Link-Layer (optional) */
    struct {
        struct icmp6_hdr icmp;
        uint8_t options[8];
    } __attribute__((packed)) rs;
    memset(&rs,0,sizeof(rs));
    rs.icmp.icmp6_type = ND_ROUTER_SOLICIT;
    rs.icmp.icmp6_code = 0;
    /* Option: Source Link-Layer address */
    rs.options[0] = 1; rs.options[1] = 1; memcpy(&rs.options[2], our_mac, 6);

    size_t icmp_len = sizeof(rs);
    uint16_t plen = htons((uint16_t)icmp_len);
    memcpy(((uint8_t*)&ip6)+4, &plen, 2);
    memcpy(frame + sizeof(struct eth_hdr), &ip6, sizeof(ip6));

    rs.icmp.icmp6_cksum = 0;
    rs.icmp.icmp6_cksum = icmpv6_checksum(&ip6, &rs.icmp, icmp_len);

    memcpy(frame+off, &rs, icmp_len); off += icmp_len;
    *flen = off;
}

/* Build Router Advertisement (RA) with Prefix Information option */
void build_ra(uint8_t *frame, size_t *flen) {
    uint8_t our_mac[6] = {0x02,0x11,0x22,0x33,0x44,0x55};
    uint8_t dst_mac[6] = {0x33,0x33,0xff,0x12,0x34,0x56}; /* example multicast */
    struct in6_addr src_ip, dst_ip;
    inet_pton(AF_INET6, "fe80::211:22ff:fe33:4455", &src_ip);
    inet_pton(AF_INET6, "ff02::1", &dst_ip); /* all-nodes multicast for example */

    size_t off=0;
    struct eth_hdr eth;
    memcpy(eth.dst, dst_mac, 6); memcpy(eth.src, our_mac, 6); eth.ethertype = htons(0x86dd);
    memcpy(frame+off, &eth, sizeof(eth)); off += sizeof(eth);

    struct ip6_hdr ip6; memset(&ip6,0,sizeof(ip6));
    ip6.ip6_flow = htonl(6<<28); ip6.ip6_nxt = IPPROTO_ICMPV6; ip6.ip6_hlim = 255;
    ip6.ip6_src = src_ip; ip6.ip6_dst = dst_ip;
    memcpy(frame+off, &ip6, sizeof(ip6)); off += sizeof(ip6);

    /* ICMPv6 RA: type 134 */
    struct {
        struct icmp6_hdr icmp;
        uint8_t cur_hop_limit;
        uint8_t flags;
        uint16_t router_lifetime;
        uint32_t reachable_time;
        uint32_t retrans_timer;
        /* options follow */
        /* we'll append one Prefix Information option (30 bytes) */
    } __attribute__((packed)) ra_hdr;

    memset(&ra_hdr,0,sizeof(ra_hdr));
    ra_hdr.icmp.icmp6_type = ND_ROUTER_ADVERT;
    ra_hdr.icmp.icmp6_code = 0;
    ra_hdr.cur_hop_limit = 64;
    ra_hdr.flags = 0x00;
    ra_hdr.router_lifetime = htons(1800);
    ra_hdr.reachable_time = htonl(0);
    ra_hdr.retrans_timer = htonl(0);

    struct {
        uint8_t type;
        uint8_t length;
        uint8_t prefix_len;
        uint8_t flags;
        uint32_t valid_lifetime;
        uint32_t preferred_lifetime;
        uint32_t reserved;
        struct in6_addr prefix;
    } __attribute__((packed)) pinfo;

    memset(&pinfo,0,sizeof(pinfo));
    pinfo.type = 3; /* Prefix Information */
    pinfo.length = 4; /* units of 8 octets -> 4*8=32 bytes */
    pinfo.prefix_len = 64;
    pinfo.flags = 0xc0; /* on-link + autonomous */
    pinfo.valid_lifetime = htonl(2592000);
    pinfo.preferred_lifetime = htonl(604800);
    inet_pton(AF_INET6, "2001:db8:1::", &pinfo.prefix);

    size_t icmp_len = sizeof(ra_hdr) + sizeof(pinfo);
    uint16_t plen = htons((uint16_t)icmp_len);
    memcpy(((uint8_t*)&ip6)+4, &plen, 2);
    memcpy(frame + sizeof(struct eth_hdr), &ip6, sizeof(ip6));

    /* compute checksum over RA */
    struct {
        struct icmp6_hdr icmp;
        uint8_t rest[sizeof(ra_hdr)-sizeof(struct icmp6_hdr)];
        uint8_t opts[sizeof(pinfo)];
    } __attribute__((packed)) tmp;
    memset(&tmp,0,sizeof(tmp));
    memcpy(&tmp.icmp, &ra_hdr.icmp, sizeof(ra_hdr.icmp));
    memcpy(&tmp.rest, ((uint8_t*)&ra_hdr)+sizeof(ra_hdr.icmp), sizeof(ra_hdr)-sizeof(ra_hdr.icmp));
    memcpy(((uint8_t*)&tmp)+sizeof(tmp)-sizeof(pinfo), &pinfo, sizeof(pinfo));

    /* place ra_hdr and pinfo into frame and compute checksum */
    ra_hdr.icmp.icmp6_cksum = 0;
    /* construct contiguous icmp data for checksum */
    size_t icmp_offset = sizeof(struct eth_hdr) + sizeof(struct ip6_hdr);
    uint8_t *icmp_ptr = frame + icmp_offset;
    memcpy(icmp_ptr, &ra_hdr, sizeof(ra_hdr));
    memcpy(icmp_ptr + sizeof(ra_hdr), &pinfo, sizeof(pinfo));
    ra_hdr.icmp.icmp6_cksum = icmpv6_checksum(&ip6, (struct icmp6_hdr *)icmp_ptr, icmp_len);
    /* write checksum back */
    memcpy(icmp_ptr + offsetof(struct icmp6_hdr, icmp6_cksum), &ra_hdr.icmp.icmp6_cksum, sizeof(ra_hdr.icmp.icmp6_cksum));

    /* finalize frame length */
    memcpy(frame + sizeof(struct eth_hdr) + sizeof(struct ip6_hdr), icmp_ptr, icmp_len);
    off += icmp_len;

    *flen = off;
}

int main(void) {
    uint8_t frame[2048];
    size_t flen;

    build_rs(frame, &flen);
    printf("=== Router Solicitation ===\n");
    simulated_send(frame, flen);

    build_ra(frame, &flen);
    printf("\n=== Router Advertisement ===\n");
    simulated_send(frame, flen);

    return 0;
}
