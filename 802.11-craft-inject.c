#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pcap.h>
#include <net/if.h> // For struct ifreq
#include <sys/ioctl.h>
#include <arpa/inet.h>

// --- 1. RADIOTAP HEADER DEFINITIONS ---
// Minimal Radiotap Header structure (only mandatory fields)
struct ieee80211_radiotap_header {
    uint8_t it_version;    // Version 0
    uint8_t it_pad;        // Padding (align on 2-byte boundary)
    uint16_t it_len;       // Total length of the header
    uint32_t it_present;   // Bitmask of fields present
} __attribute__((packed));

// Minimal Radiotap Present flags (Rate is common and simple)
#define BIT_RATE 0x04

// --- 2. 802.11 FRAME DEFINITIONS ---

// 802.11 MAC Header for a Management Frame (Beacon)
struct ieee80211_mac_header {
    uint16_t frame_control; // Type (Management), Subtype (Beacon)
    uint16_t duration_id;   // 0 for Beacon frames
    uint8_t addr1[6];       // Destination Address (Broadcast FF:FF:FF:FF:FF:FF)
    uint8_t addr2[6];       // Source Address (BSSID)
    uint8_t addr3[6];       // BSSID (Access Point MAC)
    uint16_t seq_ctrl;      // Sequence Control
} __attribute__((packed));

// Fixed fields for a Beacon frame body
struct ieee80211_beacon_fixed_params {
    uint64_t timestamp;     // 8 bytes (time since AP power-up)
    uint16_t beacon_interval; // 100 TUs (102.4 ms)
    uint16_t capabilities;  // Security and network capabilities
} __attribute__((packed));

// --- CONSTANTS ---
#define CHANNEL_FREQ_2412 2412 // Channel 1 (2.4 GHz) in MHz
#define BEACON_INTERVAL 0x0064 // 100 TUs (standard)
#define MAX_PACKET_LEN 256
#define RADIOTAP_HDR_LEN 12 // Radiotap header length for version 0, Rate field present

// Function to construct and inject the packet
void inject_packet(pcap_t *handle, const char *iface) {
    uint8_t packet[MAX_PACKET_LEN];
    int offset = 0;
    
    // --- STEP 1: RADIOTAP HEADER CONSTRUCTION ---
    
    struct ieee80211_radiotap_header *rthdr = (struct ieee80211_radiotap_header *)packet;
    rthdr->it_version = 0;
    rthdr->it_pad = 0;
    rthdr->it_len = htole16(RADIOTAP_HDR_LEN); // Use host-to-little-endian for length
    rthdr->it_present = htole32(1 << BIT_RATE); // Set Rate bit in the presence mask
    offset += 8; // Size of the base radiotap header

    // Radiotap Rate Field (802.11b 1Mbps)
    packet[offset++] = 0x02; // Rate in 0.5 Mbps units (0x02 * 0.5 = 1 Mbps)
    packet[offset++] = 0x00; // Padding (aligns the next field)

    // --- STEP 2: 802.11 MAC HEADER CONSTRUCTION ---

    struct ieee80211_mac_header *machdr = (struct ieee80211_mac_header *)(packet + offset);
    
    // Frame Control: Management (0x00) Subtype Beacon (0x08). LE: 0x80 0x00
    // 0x8000 (Host) -> 0x80 (Low byte), 0x00 (High byte) in memory (Little Endian)
    machdr->frame_control = 0x8000; 
    
    machdr->duration_id = 0x0000; // Duration 0 for Beacons
    
    // Address 1 (Destination: Broadcast)
    memset(machdr->addr1, 0xFF, 6); 
    
    // Address 2 (Source MAC/BSSID: Arbitrary MAC for our AP)
    uint8_t ap_mac[6] = {0xCA, 0xFE, 0xBA, 0xBE, 0x00, 0x01}; 
    memcpy(machdr->addr2, ap_mac, 6);
    
    // Address 3 (BSSID: Same as Source)
    memcpy(machdr->addr3, ap_mac, 6);
    
    // Sequence Control (Set to 0, though sequence numbers should be managed)
    machdr->seq_ctrl = 0x0000;
    
    offset += sizeof(struct ieee80211_mac_header);
    
    // --- STEP 3: BEACON FRAME BODY (FIXED FIELDS) ---
    
    struct ieee80211_beacon_fixed_params *fixparams = 
        (struct ieee80211_beacon_fixed_params *)(packet + offset);

    // Timestamp is ignored for simple injection, setting to 0
    fixparams->timestamp = 0x0000000000000000; 
    fixparams->beacon_interval = htole16(BEACON_INTERVAL);
    fixparams->capabilities = 0x0001; // Minimal capability (ESS)
    
    offset += sizeof(struct ieee80211_beacon_fixed_params);
    
    // --- STEP 4: BEACON FRAME BODY (INFORMATION ELEMENTS - IEs) ---
    
    // IE 0: SSID Parameter Set (Tag: 0, Length: SSID Length)
    uint8_t *ssid_tag = packet + offset;
    *ssid_tag++ = 0x00; // Tag Number: 0 (SSID)
    
    const char *ssid = "MyRawAP";
    uint8_t ssid_len = strlen(ssid);
    
    *ssid_tag++ = ssid_len; // Length
    memcpy(ssid_tag, ssid, ssid_len); // SSID string
    offset += 2 + ssid_len;

    // IE 1: Supported Rates (Tag: 1, Length: 8 bytes)
    uint8_t supported_rates[] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x0c, 0x18, 0x30, 0x48};
    memcpy(packet + offset, supported_rates, sizeof(supported_rates));
    offset += sizeof(supported_rates);
    
    // IE 3: DS Parameter Set (Tag: 3, Length: 1)
    // Indicates the current channel (e.g., Channel 1)
    uint8_t channel_ie[] = {0x03, 0x01, 0x01};
    memcpy(packet + offset, channel_ie, sizeof(channel_ie));
    offset += sizeof(channel_ie);
    
    // --- STEP 5: INJECTION ---

    printf("Injecting beacon frame (SSID: %s, Length: %d) on %s...\n", ssid, offset, iface);

    // Use pcap_sendpacket (or pcap_inject) to send the raw buffer
    if (pcap_sendpacket(handle, packet, offset) != 0) {
        fprintf(stderr, "Error injecting packet: %s\n", pcap_geterr(handle));
        return;
    }
    
    printf("Packet injected successfully.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <interface_in_monitor_mode>\n", argv[0]);
        return 1;
    }
    
    const char *iface = argv[1];
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle;

    // Open the interface for injection
    handle = pcap_open_live(iface, BUFSIZ, 0, 1000, errbuf); 
    if (handle == NULL) {
        fprintf(stderr, "Could not open interface %s: %s\n", iface, errbuf);
        fprintf(stderr, "Ensure interface is in monitor mode and you have sufficient permissions (root).\n");
        return 1;
    }
    
    // Datalink must be 802.11 plus Radiotap (DLT_IEEE802_11_RADIO)
    int link_type = pcap_datalink(handle);
    if (link_type != DLT_IEEE802_11_RADIO) {
        fprintf(stderr, "Error: Interface %s is not a Radiotap link type (%d).\n", iface, link_type);
        fprintf(stderr, "Ensure interface is in monitor mode (e.g., using 'airmon-ng start' or 'iw dev ... set type monitor').\n");
        pcap_close(handle);
        return 1;
    }

    // Loop for continuous injection (e.g., for a Beacon)
    for (int i = 0; i < 5; i++) {
        inject_packet(handle, iface);
        usleep(100000); // Wait 100ms between injections
    }

    pcap_close(handle);
    return 0;
}
