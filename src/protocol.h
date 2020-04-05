
#ifndef SDHCP_PROTOCOL
#define SDHCP_PROTOCOL 1

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h> /* The L2 protocols */
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "svdhcp.h"
#include "rsa.h"
#include "raw.h"
#include "udp.h"

/*
struct info_t {
	char mac[ETH_ALEN];
	in_addr_t ip;
	int port;
}*/


struct client_t {
    RSA *rsa;
    struct udp_t udp;
    struct raw_t raw;

    int expected_packet;

    char macaddress[ETH_ALEN];

    int sig_sockfd;

    struct client_t * next_client;
    struct server_t * list_server;

    char login[LOGIN_MAXSIZE];
    char password[LOGIN_MAXSIZE];
    char config[BUFFER_SIZE];

    long int timeout;
    int state;
    int xid;
    char packetmacaddress[BUFFER_SIZE];

    long int n;
    long int m;
    /*
     char crypt_kc_n[BUFFER_SIZE];
    int crypt_kc_n_sz;
    char odd_ks_m[BUFFER_SIZE];
    char odd_ks_sha_pass[BUFFER_SIZE];

    */
};

struct server_t {
    RSA *rsa;
    struct udp_t udp_rx;
    struct udp_t udp_tx;
    struct raw_t raw_tx;

    int expected_packet;

    char macaddress[ETH_ALEN];

    int sig_sockfd;

    struct server_t * next_server;
    struct client_t * list_client ;

    char pidfile[BUFFER_SIZE];
    int xid;
};


int broadcast_sDHCP (struct client_t * client, struct server_t * server);
int new_client (struct client_t * client,struct server_t * server, struct packet_t * packet);
int send_packet_2 (struct client_t * client, struct server_t * server );
int new_server ( struct client_t * client, struct server_t * server, struct packet_t * packet);
int send_packet_3 (struct client_t * client, struct server_t * server );
int rcv_packet_3 ( struct client_t * client, struct server_t * server, struct packet_t * packet);
int send_packet_4 (struct client_t * client, struct server_t * server );
int rcv_packet_4 ( struct client_t * client, struct server_t * server, struct packet_t * packet);
int send_packet_5 (struct client_t * client, struct server_t * server );
int rcv_packet_5 ( struct client_t * client, struct server_t * server, struct packet_t * packet);
int send_packet_6 (struct client_t * client, struct server_t * server );
int rcv_packet_6 ( struct client_t * client, struct server_t * server, struct packet_t * packet);
int send_packet_7 (struct client_t * client, struct server_t * server );
int rcv_packet_7 ( struct client_t * client, struct server_t * server, struct packet_t * packet);
int send_packet_8 (struct client_t * client, struct server_t * server );
int rcv_packet_8 ( struct client_t * client, struct server_t * server, struct packet_t * packet);

#endif
