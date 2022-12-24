
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/udp.h> //Provides declarations for udp header
#include <netinet/tcp.h> //Provides declarations for tcp header
#include <netinet/ip.h>  //Provides declarations for ip header
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <pcap.h>

#define PCAP_FILE "udp_packets.pcap"
#define PCAP_SNAPLEN 65535
#define PCAP_PROMISC 1
#define PCAP_TIMEOUT 1000

int main(int argc, char *argv[])
{
    // Create a raw socket
    int sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_IP);
    if (sock < 0)
    {
        perror("Failed to create raw socket");
        return 1;
    }

    // Bind the socket to a local address
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Failed to bind raw socket");
        return 1;
    }


    // Open a pcap file for writing
    pcap_t *pcap = pcap_open_dead(DLT_RAW, PCAP_SNAPLEN);
    if (!pcap)
    {
        fprintf(stderr, "Failed to open pcap file for writing: %s\n", PCAP_FILE);
        return 1;
    }
    pcap_dumper_t *dumper = pcap_dump_open(pcap, PCAP_FILE);
    if (!dumper)
    {
        fprintf(stderr, "Failed to open pcap dumper: %s\n", pcap_geterr(pcap));
        pcap_close(pcap);
        return 1;
    }

    // Set the pcap file header
    struct pcap_pkthdr header;
    header.caplen = PCAP_SNAPLEN;
    header.len = PCAP_SNAPLEN;
    header.ts.tv_sec = 0;
    header.ts.tv_usec = 0;

    // Receive packets and write them to the pcap file
    while (1)
    {
        // Receive a packet from the raw socket
        char buffer[PCAP_SNAPLEN];
        int bytes_recv = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes_recv < 0)
        {
            perror("Failed to receive packet from raw socket");
            break;
        }

        // Check if the packet is a UDP packet on port 80
        struct iphdr *ip_header = (struct iphdr *)buffer;
        if (ip_header->protocol != IPPROTO_UDP)
        {
            continue;
        }
        struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct iphdr));
        // if (ntohs(udp_header->dest) == 9431)
        // {
        //     continue;
        // }
        header.ts.tv_sec = time(0);
        pcap_dump((u_char *)dumper, &header, (u_char *)buffer);
    }

    // Close the pcap dumper and file
    pcap_dump_close(dumper);
    pcap_close(pcap);

    // Close the raw socket
    close(sock);

    return 0;
}