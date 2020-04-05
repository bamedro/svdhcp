#include <sys/socket.h>
//#include <netpacket/packet.h>
//#include <net/ethernet.h>     /* the L2 protocols */
#include <asm/types.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
//#include <net/if.h>
#include <netinet/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/if_ether.h> /* The L2 protocols */
#include <linux/if_arp.h>
#include <arpa/inet.h>
#include <syslog.h>

#include <openssl/engine.h>
#include <openssl/sha.h>
#include <openssl/bn.h>


#include "sdhcp.h"
#include "common.h"
#include "protocol.h"
#include "raw.h"

/*
 * int main (void)
{
	int rez;
	char toto[]="oh c bobo";
	char titi[]="oh c bibib";
	struct client_t client;
	init_raw_socket(&(client.raw), "eth0",PORT_SERVER,PORT_CLIENT);	
	send_raw_socket(&(client.raw), toto, sizeof("oh c bobo"));
	chg_header_raw_socket(&(client.raw),"012345");
	send_raw_socket(&(client.raw), titi, sizeof("oh c bibib"));
	return 1;
}
*/
//Send data threw raw socket
int send_raw_socket( struct raw_t * raw, char * data, int sz)
{
	int rez;
	
	if(sz + sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr) > RAW_FRAME_LENGTH)
	{
                LOG(LOG_CRIT,"Send raw socket - buffer too small");
		return -ERR_SOCKET;
	}
	
	memcpy(raw->data, data, sz);
	raw->iphead->tot_len = htons(sizeof(struct iphdr) + sizeof(struct udphdr) + sz);
	raw->udphead->len = htons(sizeof(struct udphdr) + sz);
	raw->iphead->check = 0;
	raw->iphead->check = cksum( (unsigned short *)raw->iphead, sizeof(struct iphdr));
	
	rez= sendto(raw->sockfd, raw->buffer, sz + sizeof(struct ethhdr) + sizeof(struct iphdr) +sizeof(struct udphdr), 0, (struct sockaddr *)(&(raw->saddr)), sizeof(struct sockaddr_ll));	
	if (rez == -1)
	{
		perror("sendto raw socket");
                LOG(LOG_CRIT,"Unable to send packet with raw socket");
		return -ERR_SOCKET;
	}
	return 1;
}

// Change destination mac address
int chg_header_raw_socket (struct raw_t * raw, char * macdest)
{	
	raw->saddr.sll_addr[0] = macdest[0];
	raw->saddr.sll_addr[1] = macdest[1];
	raw->saddr.sll_addr[2] = macdest[2];
	raw->saddr.sll_addr[3] = macdest[3];
	raw->saddr.sll_addr[4] = macdest[4];
	raw->saddr.sll_addr[5] = macdest[5];	
	memcpy(raw->ethhead->h_dest, raw->saddr.sll_addr, ETH_ALEN);
}

unsigned short cksum ( unsigned short * data, int len)
{
	unsigned long checksum = 0;

	while(len > 1)
	{
		checksum = checksum + *data++;
		len = len - sizeof(unsigned short);
	}
	
	if(len)
		checksum = checksum +*(unsigned char*)data;

	checksum = (checksum>>16) + (checksum&0xffff);
	checksum = checksum + (checksum>>16);
	
	return (unsigned short) (~checksum);
}
// Initialize raw socket
int init_raw_socket (struct raw_t * raw, char * devname, int portin, int portout)
{
	bzero(raw->buffer, RAW_FRAME_LENGTH);
	// Open packet
	raw->sockfd = open_raw_socket();
	if(raw->sockfd == -1)
		return -1;

	raw->saddr.sll_family = AF_PACKET;
	//raw->saddr.sll_hatype = ARPHRD_ETHER;
	//raw->saddr.sll_pkttype = PACKET_OTHERHOST;
	raw->saddr.sll_protocol = htons(ETH_P_IP);
	
	// Lock to an interface
	raw->saddr.sll_ifindex = get_ifindex(raw->sockfd, devname);
	raw->saddr.sll_halen = ETH_ALEN;

	// Default Mac destination
	raw->saddr.sll_addr[0] = 0xff;
	raw->saddr.sll_addr[1] = 0xff;
	raw->saddr.sll_addr[2] = 0xff;
	raw->saddr.sll_addr[3] = 0xff;
	raw->saddr.sll_addr[4] = 0xff;
	raw->saddr.sll_addr[5] = 0xff;
	
	// Not used
	raw->saddr.sll_addr[6] = 0x00;
	raw->saddr.sll_addr[7] = 0x00;

	raw->ethhead = (void *)raw->buffer;
	raw->iphead = (void *) raw->buffer + sizeof(struct ethhdr);
	raw->udphead = (void *)raw->iphead + sizeof(struct iphdr);
	raw->data = (void * )raw->udphead + sizeof(struct udphdr);
	
	memcpy(raw->ethhead->h_dest, raw->saddr.sll_addr, ETH_ALEN);
	get_hwaddr(raw->ethhead->h_source, raw->sockfd, devname);
	raw->ethhead->h_proto = htons(ETH_P_IP);
	//Version
	raw->iphead->version = 4;
	//Header length
	raw->iphead->ihl = 5;
	raw->iphead->tos = 0 ;
	raw->iphead->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr);
	raw->iphead->id = 1 ;
	raw->iphead->frag_off = 0 ;
	raw->iphead->ttl = 255;
	raw->iphead->protocol = IPPROTO_UDP;
	raw->iphead->saddr = get_ipaddress(raw->sockfd, devname);
//	raw->iphead->saddr = inet_addr("192.168.0.1");
	raw->iphead->daddr = inet_addr("255.255.255.255");
//	raw->iphead->check = cksum( (unsigned short *)raw->iphead, sizeof(struct iphdr));
	raw->iphead->check = 0;
	raw->udphead->source = htons(portin) ;
	raw->udphead->dest = htons(portout);
	raw->udphead->len = sizeof(struct udphdr);
	raw->udphead->check = 0;

	return raw->sockfd;
}

// Open raw socket
int open_raw_socket(void)
{
	int packet_socket;
	struct ifreq ifr;
	int rez;
	
	
	 packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	 if(packet_socket == -1)
	 {
		 perror("packet socket");
                 LOG(LOG_CRIT,"Unable to open packet socket");
		 return -ERR_SOCKET;
	 }
    	LOG(LOG_INFO,"Opening raw socket (AF_PACKET)");
	 return packet_socket;
	}

// Get ip addres of a network device
in_addr_t get_ipaddress(int sockfd, char * devname)
{
	struct ifreq ifr;
	struct sockaddr_in * sin;
	int rez=1;
	char * ipbuffer;

	strcpy(ifr.ifr_name,devname);
	rez = ioctl(sockfd,SIOCGIFADDR,&ifr);
	 if(rez)
	 {
		 perror("ioctl SIOCGIFADDR");
                 LOG(LOG_CRIT,"Unable to get ip address of %s",devname);
		 return inet_addr("255.255.255.255");
	}
	 sin = (struct sockaddr_in *)(&(ifr.ifr_addr));
	ipbuffer = inet_ntoa( sin->sin_addr);
    	LOG(LOG_INFO,"Getting ipaddress of %s: %s", ifr.ifr_name,ipbuffer);
//	free(ipbuffer);
	return sin->sin_addr.s_addr;
}

// Get ifindex of a network device
int get_ifindex(int sockfd, char * devname)
{
	struct ifreq ifr;
	int rez;

	strcpy(ifr.ifr_name,devname);
	rez = ioctl(sockfd,SIOCGIFINDEX,&ifr);
	 if(rez)
	 {
		 perror("ioctl SIOCGIFINDEX");
                 LOG(LOG_CRIT,"Unable to get ifindex");
		 return -ERR_SOCKET;
	}

    	LOG(LOG_INFO,"Getting network interface %s(%d)", ifr.ifr_name, ifr.ifr_ifindex);
	return ifr.ifr_ifindex;
}

// Get HWaddr of a network device
int get_hwaddr (char * hwaddr, int sockfd, char * devname)
{	
	struct ifreq ifr;
	int rez;
	strcpy(ifr.ifr_name,devname);
	rez = ioctl(sockfd,SIOCGIFHWADDR,&ifr);
	 if(rez)
	 {
		 perror("ioctl SIOCGIFINDEX");
                 LOG(LOG_CRIT,"Unable to get hw addr");
		 return -ERR_SOCKET;
	}	
    	LOG(LOG_INFO,"Getting hwaddr of %s: %02X:%02X:%02X:%02X:%02X:%02X", ifr.ifr_name, (unsigned char) ifr.ifr_hwaddr.sa_data[0], (unsigned char) ifr.ifr_hwaddr.sa_data[1],	(unsigned char) ifr.ifr_hwaddr.sa_data[2], (unsigned char) ifr.ifr_hwaddr.sa_data[3], (unsigned char) ifr.ifr_hwaddr.sa_data[4], (unsigned char) ifr.ifr_hwaddr.sa_data[5]);
	memcpy(hwaddr,ifr.ifr_hwaddr.sa_data,6);
}
