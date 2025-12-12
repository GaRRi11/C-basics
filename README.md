-------------------------------------------------------------------------------- UNIX -------------------------------------------------------------------------------------




## OSI LAYER MODEL

OSI defines levels clearly; every upper layer is built on a lower layer and uses it to operate.

7. APPLICATION

Applications with UI, this layer uses all 6 lower layers.

6. PRESENTATION

At this level, data structure is defined, data encoding, serialization, encryption.

5. SESSION

Uses transport layer and adds methods to establish, suspend, checkpoint, resume, and terminate dialogs.

4. TRANSPORT

TCP/UDP methods to deliver data between hosts; these methods split up data, recombine it, ensure it arrived, etc.

3. NETWORK

Transmit packets in different networks; this layer is used by Internet Protocol.

2. DATA LINK

Deals with protocols for directly communicating between two nodes. Defines how a direct message between nodes starts and ends (framing), error detection and correction, and flow control. 802.11 (WiFi) or Ethernet frame (wired).

1. PHYSICAL

Ethernet cable voltage, WiFi radio frequency, light flashes over optical fiber. Radio waves (WiFi) or signals (wired).



## Encapsulation / Decapsulation

Each upper layer has its own message only and has info about what to send to the next lower layer. On each layer, more info is added to data about the next hop. That’s called encapsulation.

When a packet reaches its destination, decapsulation starts — each layer reads and removes its header.

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------


## TCP/IP LAYER MODEL

4. PROCESS/APPLICATION

The process/application layer is where protocols such as HTTP, SMTP, and FTP are implemented.

3. HOST TO HOST

TCP/UDP.

2. INTERNET

This layer deals with addressing packets and routing them over multiple networks.

1. NETWORK ACCESS

Ethernet cable voltage, WiFi radio frequency, light flashes over optical fiber. Radio waves (WiFi) or signals (wired).


---------------------------------------------------------------------------------------------------------------------------------------------------------------------------


LAN PROTOCOLS AND THEIR LAYERS:

| Protocol                                        | Layer                                                                | Description                                                            |
| ----------------------------------------------- | -------------------------------------------------------------------- | ---------------------------------------------------------------------- |
| **ARP** (Address Resolution Protocol)           | **Layer 2 (Link Layer)**                                             | Resolves IPv4 → MAC. Runs directly over Ethernet. Not an IP protocol.  |
| **NDP** (NS/NA, RS/RA, Redirect, etc. for IPv6) | **Layer 3 (Network Layer)**                                          | Neighbor Discovery Protocol. Built on **ICMPv6** which runs over IPv6. |
| **DHCPv4**                                      | **Layer 7 (Application)** but carried by **UDP (Layer 4)** over IPv4 | Used for IPv4 address assignment.                                      |
| **DHCPv6**                                      | **Layer 7 (Application)** carried by **UDP** over IPv6               | IPv6 version of DHCP.                                                  |
| **ICMPv4**                                      | **Layer 3 (Network)** over IPv4                                      | Ping, errors, etc.                                                     |
| **ICMPv6**                                      | **Layer 3 (Network)** over IPv6                                      | Required for IPv6. NDP uses this.                                      |



---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
