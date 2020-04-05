
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h> /* The L2 protocols */
#include <openssl/rsa.h>
#include <syslog.h>
#include "sdhcp.h"
#include "protocol.h"
#include "rsa.h"
#include "common.h" 

int config_load_client(struct client_t * client)
{
	return 1;
}
int config_load_server(struct server_t * server)
{

    // Initialize log and write filepid 
    start_log_and_pid("sdhcpd","/var/run/sdhcpd.pid");
    
    // Initialize rsa structure
    RSA_init_key(&server->rsa);
}
