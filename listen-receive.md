c raw packet listener

// gcc sniffer.c -o sniffer
// sudo ./sniffer

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip6.h>
#include <netinet/icmp6.h>
#include <netinet/if_ether.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    // Create RAW socket for ALL Ethernet protocols
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    printf("Listening for ARP and ICMPv6 Neighbor Solicitation...\n");

    uint8_t buffer[2048];

    while (1) {
        ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) continue;

        struct ethhdr *eth = (struct ethhdr *)buffer;

        // ----------- ARP -----------------------------------
        if (ntohs(eth->h_proto) == ETH_P_ARP) {
            struct ether_arp *arp = (struct ether_arp *)(buffer + sizeof(struct ethhdr));

            uint8_t *spa = arp->arp_spa;
            uint8_t *tpa = arp->arp_tpa;

            printf("\n[ARP] Who-has %d.%d.%d.%d? Tell %d.%d.%d.%d\n",
                   tpa[0], tpa[1], tpa[2], tpa[3],
                   spa[0], spa[1], spa[2], spa[3]);
        }

        // ----------- IPv6 ICMPv6 ---------------------------
        if (ntohs(eth->h_proto) == ETH_P_IPV6) {
            struct ip6_hdr *ip6 = (struct ip6_hdr *)(buffer + sizeof(struct ethhdr));

            if (ip6->ip6_nxt == IPPROTO_ICMPV6) {
                struct icmp6_hdr *icmp =
                    (struct icmp6_hdr *)((uint8_t *)ip6 + sizeof(struct ip6_hdr));

                // ICMPv6 Type 135 = Neighbor Solicitation
                if (icmp->icmp6_type == ND_NEIGHBOR_SOLICIT) {
                    struct nd_neighbor_solicit *ns =
                        (struct nd_neighbor_solicit *)icmp;

                    char target[INET6_ADDRSTRLEN];
                    inet_ntop(AF_INET6, &ns->nd_ns_target, target, sizeof(target));

                    char src[INET6_ADDRSTRLEN];
                    inet_ntop(AF_INET6, &ip6->ip6_src, src, sizeof(src));

                    printf("\n[ICMPv6 NS] %s asking for %s\n", src, target);
                }
            }
        }
    }

    close(sock);
    return 0;
}





responder


/*
 * responder.c -- Safe ARP + IPv6 Neighbor Advertisement responder
 *
 * Compile:
 *   gcc responder.c -o responder
 *
 * Run (as root):
 *   sudo ./responder
 *
 * Behavior:
 * - Replies to ARP requests for IPv4 addresses actually assigned to the host.
 * - Replies to ICMPv6 Neighbor Solicitation (type 135) for IPv6 addresses
 *   actually assigned to the host (typically link-local fe80::/10).
 *
 * Note: Linux-only (AF_PACKET / sockaddr_ll). Use only on test / lab networks.
 */

#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <net/if.h>
#include <netinet/icmp6.h>
#include <netinet/ip6.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/* sizes */
#define ETH_HDR_LEN 14
#define ARP_PKT_LEN 28   /* standard ARP payload size */
#define ETH_FRAME_MAX 1522

/* A small structure to hold interface info we need */
typedef struct {
    char ifname[IFNAMSIZ];
    unsigned int ifindex;
    uint8_t mac[6];            /* MAC (or zeros if none) */
    int has_mac;
    struct in_addr ipv4;       /* store only one IPv4 per iface for simplicity; INADDR_ANY if none */
    int has_ipv4;
    struct in6_addr ipv6_ll;   /* store one link-local IPv6 if present */
    int has_ipv6_ll;
} iface_info_t;

/* We'll gather all interfaces into a dynamic array */
iface_info_t *ifaces = NULL;
size_t ifaces_count = 0;

/* Helper: print MAC */
static void mac_to_str(const uint8_t *mac, char *buf, size_t buflen) {
    snprintf(buf, buflen, "%02x:%02x:%02x:%02x:%02x:%02x",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/* Compute IPv6 pseudo-header checksum helper (for ICMPv6) */
static uint16_t csum16(const void *buf, size_t len) {
    const uint8_t *data = buf;
    uint32_t sum = 0;
    while (len > 1) {
        sum += (data[0] << 8) | data[1];
        data += 2;
        len -= 2;
    }
    if (len) {
        sum += data[0] << 8;
    }
    /* add carries */
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return (uint16_t)~sum;
}

/* Compute ICMPv6 checksum over pseudo-header + icmpv6 */
static uint16_t icmpv6_checksum(const struct ip6_hdr *ip6, const struct icmp6_hdr *icmp6, size_t icmp_len) {
    /* pseudo-header: src(16) + dst(16) + len(4) + zero(3) + next_header(1) */
    uint32_t icmp_len32 = htonl((uint32_t)icmp_len);
    uint8_t pseudo[16 + 16 + 4 + 4]; /* src + dst + len + (zero+next) */
    uint8_t *p = pseudo;
    memcpy(p, &ip6->ip6_src, 16); p += 16;
    memcpy(p, &ip6->ip6_dst, 16); p += 16;
    memcpy(p, &icmp_len32, 4); p += 4;
    memset(p, 0, 3); p += 3;
    *p = IPPROTO_ICMPV6; /* next header */
    /* Now compute sum over pseudo + icmp */
    uint32_t sum = 0;
    sum += csum16(pseudo, sizeof(pseudo));
    sum += csum16(icmp6, icmp_len);
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return (uint16_t)~sum;
}

/* Gather interface info: ifindex, mac, one IPv4, one IPv6 link-local */
static void gather_interfaces() {
    struct ifaddrs *ifa, *p;
    if (getifaddrs(&ifa) < 0) {
        perror("getifaddrs");
        exit(1);
    }

    /* First pass: create entries for unique ifnames */
    for (p = ifa; p; p = p->ifa_next) {
        if (!p->ifa_name) continue;
        /* find existing */
        size_t i;
        for (i = 0; i < ifaces_count; ++i) {
            if (strcmp(ifaces[i].ifname, p->ifa_name) == 0) break;
        }
        if (i == ifaces_count) {
            /* add new entry */
            ifaces = realloc(ifaces, (ifaces_count + 1) * sizeof(iface_info_t));
            memset(&ifaces[ifaces_count], 0, sizeof(iface_info_t));
            strncpy(ifaces[ifaces_count].ifname, p->ifa_name, IFNAMSIZ-1);
            ifaces[ifaces_count].ifindex = if_nametoindex(p->ifa_name);
            ifaces[ifaces_count].has_mac = 0;
            ifaces[ifaces_count].has_ipv4 = 0;
            ifaces[ifaces_count].has_ipv6_ll = 0;
            ifaces_count++;
        }
    }

    /* Second pass: fill addresses and MACs */
    for (p = ifa; p; p = p->ifa_next) {
        if (!p->ifa_name) continue;
        /* find index */
        size_t idx;
        for (idx = 0; idx < ifaces_count; ++idx) if (strcmp(ifaces[idx].ifname, p->ifa_name) == 0) break;
        if (idx == ifaces_count) continue;

        if (p->ifa_addr == NULL) continue;
        if (p->ifa_addr->sa_family == AF_PACKET) {
            struct sockaddr_ll *sll = (struct sockaddr_ll *)p->ifa_addr;
            if (sll->sll_halen >= 6) {
                memcpy(ifaces[idx].mac, sll->sll_addr, 6);
                ifaces[idx].has_mac = 1;
            }
        } else if (p->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *sin = (struct sockaddr_in *)p->ifa_addr;
            ifaces[idx].ipv4 = sin->sin_addr;
            ifaces[idx].has_ipv4 = 1;
        } else if (p->ifa_addr->sa_family == AF_INET6) {
            struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)p->ifa_addr;
            /* record link-local only (fe80::/10) */
            uint8_t *addr = (uint8_t *)&sin6->sin6_addr;
            if ((addr[0] == 0xfe) && ((addr[1] & 0xc0) == 0x80)) {
                ifaces[idx].ipv6_ll = sin6->sin6_addr;
                ifaces[idx].has_ipv6_ll = 1;
            }
        }
    }

    freeifaddrs(ifa);

    /* print info */
    printf("Discovered interfaces:\n");
    for (size_t i = 0; i < ifaces_count; ++i) {
        char macbuf[32] = "none";
        if (ifaces[i].has_mac) mac_to_str(ifaces[i].mac, macbuf, sizeof(macbuf));
        char ipv4buf[INET_ADDRSTRLEN] = "-";
        if (ifaces[i].has_ipv4) inet_ntop(AF_INET, &ifaces[i].ipv4, ipv4buf, sizeof(ipv4buf));
        char ipv6buf[INET6_ADDRSTRLEN] = "-";
        if (ifaces[i].has_ipv6_ll) inet_ntop(AF_INET6, &ifaces[i].ipv6_ll, ipv6buf, sizeof(ipv6buf));
        printf(" %s idx=%u mac=%s ipv4=%s ipv6_ll=%s\n",
               ifaces[i].ifname, ifaces[i].ifindex, macbuf, ipv4buf, ipv6buf);
    }
}

/* Find iface_info index by ifindex */
static ssize_t find_iface_by_index(unsigned int ifindex) {
    for (size_t i = 0; i < ifaces_count; ++i) {
        if (ifaces[i].ifindex == ifindex) return (ssize_t)i;
    }
    return -1;
}

/* Build and send an ARP reply (Ethernet + ARP) */
static void send_arp_reply(int sock, const uint8_t *dst_mac, const uint8_t *src_mac,
                           unsigned int ifindex, const uint8_t *sender_mac,
                           const struct in_addr *sender_ip, const uint8_t *target_mac,
                           const struct in_addr *target_ip)
{
    uint8_t frame[ETH_FRAME_MAX];
    ssize_t frame_len = 0;

    /* Ethernet header */
    struct ether_header *eth = (struct ether_header *)frame;
    memcpy(eth->ether_dhost, dst_mac, 6);          /* destination (requestor MAC) */
    memcpy(eth->ether_shost, src_mac, 6);          /* source (our MAC) */
    eth->ether_type = htons(ETH_P_ARP);
    frame_len += ETH_HDR_LEN;

    /* ARP payload (Ethernet + IPv4) */
    struct ether_arp arp;
    arp.arp_hrd = htons(ARPHRD_ETHER);
    arp.arp_pro = htons(ETH_P_IP);
    arp.arp_hln = 6;
    arp.arp_pln = 4;
    arp.arp_op  = htons(ARPOP_REPLY);
    memcpy(arp.arp_sha, sender_mac, 6);            /* sender hardware (our MAC) */
    memcpy(arp.arp_spa, &sender_ip->s_addr, 4);    /* sender protocol (our IPv4) */
    memcpy(arp.arp_tha, target_mac, 6);            /* target hardware (requestor MAC) */
    memcpy(arp.arp_tpa, &target_ip->s_addr, 4);    /* target protocol (requestor IPv4) */

    memcpy(frame + frame_len, &arp, sizeof(arp));
    frame_len += sizeof(arp);

    /* send via sockaddr_ll */
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifindex;
    sll.sll_halen = 6;
    memcpy(sll.sll_addr, dst_mac, 6); /* dest MAC (requestor) */

    ssize_t sent = sendto(sock, frame, frame_len, 0, (struct sockaddr *)&sll, sizeof(sll));
    if (sent < 0) perror("sendto(arp reply)");
    else printf("Sent ARP reply (%zd bytes) on ifindex %u\n", sent, ifindex);
}

/* Build and send ICMPv6 Neighbor Advertisement */
static void send_na(int sock, const uint8_t *dst_mac, const uint8_t *src_mac,
                    unsigned int ifindex, const struct in6_addr *tgt_addr,
                    const struct in6_addr *dst_addr_ip, int solicited)
{
    /* We'll craft: Ethernet | IPv6 header | ICMPv6 NA + option (TLLA) */
    uint8_t frame[ETH_FRAME_MAX];
    memset(frame, 0, sizeof(frame));
    size_t offset = 0;

    /* Ethernet header */
    struct ether_header *eth = (struct ether_header *)frame;
    memcpy(eth->ether_dhost, dst_mac, 6);        /* destination MAC (requestor) */
    memcpy(eth->ether_shost, src_mac, 6);        /* source MAC (our MAC) */
    eth->ether_type = htons(ETH_P_IPV6);
    offset += ETH_HDR_LEN;

    /* IPv6 header */
    struct ip6_hdr *ip6 = (struct ip6_hdr *)(frame + offset);
    ip6->ip6_flow = htonl((6 << 28)); /* version=6, traffic class/flowlabel = 0 */
    ip6->ip6_plen = 0; /* will fill later */
    ip6->ip6_nxt = IPPROTO_ICMPV6;
    ip6->ip6_hlim = 255; /* per RFC 4861 */
    ip6->ip6_src = *tgt_addr;    /* source = target address (we are the target) */
    ip6->ip6_dst = *dst_addr_ip; /* destination = the original NS source (or solicited-node multicast) */
    offset += sizeof(struct ip6_hdr);

    /* ICMPv6 NA */
    struct nd_neighbor_advert {
        struct icmp6_hdr icmp6;
        uint32_t        reserved_flags;
        struct in6_addr target;
        /* options follow; we will append: type=2 length=1 (8 bytes) + link-layer 6 bytes -> total 8 bytes option field (8 bytes rounded) */
    } __attribute__((packed));

    struct nd_neighbor_advert na;
    memset(&na, 0, sizeof(na));
    na.icmp6.icmp6_type = ND_NEIGHBOR_ADVERT; /* 136 */
    na.icmp6.icmp6_code = 0;
    na.icmp6.icmp6_cksum = 0; /* calculate later */
    /* set flags in high-order byte: R, S, O bits (RFC 4861)
       we'll set Router=0, Solicited = solicited argument, Override = 1 */
    uint32_t flags = 0;
    if (solicited) flags |= (1 << 31); /* S bit in high-order position (bit 31) */
    flags |= (1 << 30); /* O bit (override) -> set */
    na.reserved_flags = htonl(flags);
    na.target = *tgt_addr;

    /* copy icmp header+reserved+target to frame */
    memcpy(frame + offset, &na, sizeof(na));
    offset += sizeof(na);

    /* Option: Target Link-Layer Address (Type=2, Length=1 (units of 8 octets)) */
    uint8_t tlla_opt[8];
    tlla_opt[0] = 2; /* type */
    tlla_opt[1] = 1; /* length = 1 (8 bytes) */
    /* next 6 bytes = our MAC */
    /* we expect src_mac holds our MAC */
    memcpy(&tlla_opt[2], src_mac, 6);
    memcpy(frame + offset, tlla_opt, sizeof(tlla_opt));
    offset += sizeof(tlla_opt);

    /* fill IPv6 payload length = ICMPv6 (na + option) */
    uint16_t payload_len = htons((uint16_t)(sizeof(na) + sizeof(tlla_opt)));
    memcpy(&ip6->ip6_plen, &payload_len, 2);

    /* compute ICMPv6 checksum (pseudo-header required) */
    struct icmp6_hdr *icmp = (struct icmp6_hdr *)(frame + ETH_HDR_LEN + sizeof(struct ip6_hdr));
    size_t icmp_len = sizeof(na) + sizeof(tlla_opt);
    uint16_t csum = icmpv6_checksum(ip6, icmp, icmp_len);
    icmp->icmp6_cksum = csum;

    /* send using sockaddr_ll */
    struct sockaddr_ll sll;
    memset(&sll, 0, sizeof(sll));
    sll.sll_family = AF_PACKET;
    sll.sll_ifindex = ifindex;
    sll.sll_halen = 6;
    memcpy(sll.sll_addr, dst_mac, 6);

    ssize_t sent = sendto(sock, frame, offset, 0, (struct sockaddr*)&sll, sizeof(sll));
    if (sent < 0) perror("sendto(na)");
    else printf("Sent NA (%zd bytes) on ifindex %u\n", sent, ifindex);
}

/* main loop: sniff and respond */
int main(void) {
    printf("Safe ARP + NDP responder (Linux). Must run as root.\n");

    gather_interfaces();

    /* open raw socket to capture Ethernet frames */
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock < 0) {
        perror("socket(AF_PACKET)");
        exit(1);
    }

    uint8_t buf[ETH_FRAME_MAX];
    while (1) {
        ssize_t n = recv(sock, buf, sizeof(buf), 0);
        if (n <= 0) {
            if (errno == EINTR) continue;
            perror("recv");
            break;
        }
        if ((size_t)n < ETH_HDR_LEN) continue;

        struct ether_header *eth = (struct ether_header *)buf;
        uint16_t ethertype = ntohs(eth->ether_type);

        /* If ARP packet */
        if (ethertype == ETH_P_ARP) {
            if ((size_t)n < ETH_HDR_LEN + sizeof(struct ether_arp)) continue;
            struct ether_arp *arp = (struct ether_arp *)(buf + ETH_HDR_LEN);
            uint16_t op = ntohs(arp->arp_op);
            if (op != ARPOP_REQUEST) continue; /* only respond to ARP requests */

            /* read target IP from arp->arp_tpa */
            struct in_addr target_ip;
            memcpy(&target_ip.s_addr, arp->arp_tpa, 4);

            /* find interface by ifindex: Extract from sockaddr_ll in recv? Unfortunately recv() doesn't give sockaddr info.
               Simpler approach: use destination MAC to locate which interface matches source MAC? Better: use packet_mreq? 
               For simplicity: we'll search ifaces for matching MAC == our MAC, and match target IP. But we also need interface index.
               We can instead parse the socket ancillary data — but that's more code.
               Good-enough approach: check each iface; if iface has_ipv4 and equals target_ip and packet was received on that iface -> respond.
               To know which interface the packet arrived on, we can use recvmsg with PACKET_AUXDATA or SOCK_RAW bound to interface. 
               To keep code readable and not too long, we'll simply attempt to match by target_ip and the arp->arp_tha (sender MAC) for reply destination, and choose iface with matching ipv4.
            */
            /* Find an iface with matching IPv4 target */
            ssize_t idx = -1;
            for (size_t i = 0; i < ifaces_count; ++i) {
                if (ifaces[i].has_ipv4) {
                    if (memcmp(&ifaces[i].ipv4.s_addr, &target_ip.s_addr, 4) == 0) { idx = i; break; }
                }
            }
            if (idx == -1) continue; /* not for us */

            /* prepare reply */
            uint8_t requester_mac[6];
            memcpy(requester_mac, arp->arp_sha, 6);
            uint8_t our_mac[6];
            memcpy(our_mac, ifaces[idx].mac, 6);

            /* our ip = sender for reply */
            struct in_addr our_ip = ifaces[idx].ipv4;

            send_arp_reply(sock, requester_mac, our_mac, ifaces[idx].ifindex,
                           our_mac, &our_ip, requester_mac, &arp->arp_spa ? (struct in_addr *)&arp->arp_spa : &arp->arp_spa);
            /* Note: the last arg (target_ip) passed not used here as we computed above */
        }

        /* If IPv6 packet */
        if (ethertype == ETH_P_IPV6) {
            if ((size_t)n < ETH_HDR_LEN + sizeof(struct ip6_hdr)) continue;
            struct ip6_hdr *ip6 = (struct ip6_hdr *)(buf + ETH_HDR_LEN);

            if (ip6->ip6_nxt != IPPROTO_ICMPV6) continue;
            /* compute pointer to icmp6 header */
            struct icmp6_hdr *icmp6 = (struct icmp6_hdr *)((uint8_t*)ip6 + sizeof(struct ip6_hdr));
            if ((size_t)n < ETH_HDR_LEN + sizeof(struct ip6_hdr) + sizeof(struct icmp6_hdr)) continue;

            if (icmp6->icmp6_type != ND_NEIGHBOR_SOLICIT) continue;

            /* ns structure: icmp6_hdr + 4 bytes reserved + target address */
            if ((size_t)n < ETH_HDR_LEN + sizeof(struct ip6_hdr) + sizeof(struct icmp6_hdr) + 4 + sizeof(struct in6_addr)) continue;
            uint8_t *ns_ptr = (uint8_t *)icmp6;
            struct in6_addr target;
            memcpy(&target, ns_ptr + sizeof(struct icmp6_hdr) + 4, sizeof(struct in6_addr));

            /* find iface by ip6 dest? Need to know which interface the frame arrived on.
               As above, recv() doesn't provide per-packet ifindex. For simplicity we search which iface has target == assigned IPv6 LL.
             */
            ssize_t idx = -1;
            for (size_t i = 0; i < ifaces_count; ++i) {
                if (ifaces[i].has_ipv6_ll) {
                    if (memcmp(&ifaces[i].ipv6_ll, &target, sizeof(target)) == 0) { idx = i; break; }
                }
            }
            if (idx == -1) continue; /* not for us */

            /* requester MAC = eth->ether_shost */
            uint8_t requester_mac[6];
            memcpy(requester_mac, eth->ether_shost, 6);
            uint8_t our_mac[6];
            memcpy(our_mac, ifaces[idx].mac, 6);

            /* Prepare destination IPv6 address for NA:
             * If NS was to solicited-node multicast (ff02::1:ffXX:XXXX), the source might be unspecified :: (for DAD)
             * If source is :: (zero), RFC says NA should be sent to the all-nodes multicast? For DAD, we should not reply.
             * We will only reply if the NS source is not :: (i.e., some real node requests).
             */
            struct in6_addr src_ip = ip6->ip6_src;
            struct in6_addr dst_ip;
            int reply_to_multicast = 0;

            /* If source is unspecified (::), it's DAD; do not reply */
            static const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
            if (memcmp(&src_ip, &in6addr_any, sizeof(in6addr_any)) == 0) {
                /* Duplicate Address Detection - do not reply */
                continue;
            }

            /* Otherwise reply to source unicast */
            dst_ip = src_ip;

            /* send NA; set solicited flag = 1 (since source is unicast and asked for it) */
            send_na(sock, requester_mac, our_mac, ifaces[idx].ifindex, &ifaces[idx].ipv6_ll, &dst_ip, 1);
        }
    }

    close(sock);
    return 0;
}



Explanations, limitations and notes

This program is intentionally conservative:

It replies only if the request's target IP equals an address our program discovered as owned by an interface.

For IPv6, it only stores and responds for link-local addresses (fe80::/10) discovered by getifaddrs—these are the addresses most NDP traffic uses on LANs.

Interface detection:

The program uses getifaddrs() to gather one IPv4 and one link-local IPv6 per interface plus the interface MAC. It prints discovered interfaces at start.

Simplifications:

The code shows one IPv4 and one link-local IPv6 per interface. Real machines may have multiple addresses; you can extend the arrays if needed.

The code uses recv() on a single raw socket and does not extract the per-packet interface index reliably; in production sniffers you'd use recvmsg() with PACKET_AUXDATA to learn the ifindex on which a frame arrived (or bind raw socket per-interface). The code works for many lab setups where the target IP uniquely identifies the interface.

ICMPv6 checksum:

The code computes ICMPv6 checksum with a pseudo-header.

DAD (Duplicate Address Detection):

If an NS has source :: (unspecified), it is a DAD probe; we do not reply. This is compliant behavior.

Root required:

Raw socket requires root privileges. Run with sudo.

Testing:

Best tested on an isolated LAN or a VM host-only network.

Monitor with tcpdump -i <iface> -n arp or icmp6 while running.
