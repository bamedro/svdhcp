#ifndef SDHCP_RAW
#define SDHCP_RAW 1


struct raw_t {
	int sockfd;
    	struct sockaddr_ll saddr;
	char buffer[RAW_FRAME_LENGTH];
	struct ethhdr * ethhead;	
	struct iphdr * iphead;
	struct udphdr * udphead;
	char * data;
};

int chg_header_raw_socket (struct raw_t * raw, char * macdest);
int open_raw_socket(void);
int init_raw_socket (struct raw_t * raw, char * devname, int portin, int portout);
in_addr_t get_ipaddress(int sockfd, char * devname);
int get_ifindex(int sockfd, char * devname);
int get_hwaddr (char * hwaddr, int sockfd, char * devname);
int send_raw_socket( struct raw_t * raw, char * data, int sz);
unsigned short cksum ( unsigned short * data, int len);


#endif
