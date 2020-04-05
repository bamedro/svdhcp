/*
 *
 * Johann Baudy <johaahn@users.sourceforge.net> 2005
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <sys/socket.h>
#include <asm/types.h>
#include <linux/if_packet.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <linux/if_ether.h> /* The L2 protocols */
#include <arpa/inet.h>
#include <openssl/rsa.h>
//#include <openssl/engine.h>
//#include <openssl/sha.h>
//#include <openssl/bn.h>


#include "sdhcp.h"
#include "rsa.h"
#include "protocol.h"
#include "raw.h"
#include "packet.h"
#include "common.h"
#include "signalpipe.h"

int timeout_check_server(struct client_t * client)
{
	return 1;
}
/* Timeout client check */
int timeout_check_client(struct server_t * server)
{
	struct client_t * client;
	struct client_t * last_client;
	struct client_t * next_client;

	client = server->list_client;
	if(client != NULL)
		while((client->next_client != NULL ))
		{
			//printf("xid = %d\n",client->xid);
			//End of list
			//printf("%d %d\n",client->timeout, time(NULL));
			if(client->timeout < time(NULL))
			{
				LOG(LOG_INFO,"timeout (id=%d, login=%s, mac=%s, pmac=%s)",client->xid,client->login, client->macaddress,client->packetmacaddress);
				//Close client socket
				//close(client->udp->sockfd);
				//Free rsa structure
				free(client->rsa);
				//Restore list links
				if(server->list_client == client)
				{
					server->list_client = client->next_client;
					//Free client structure
					free(client);
					client = server->list_client;
				}
				else
				{
					last_client->next_client = client->next_client;
					free(client);
					client = last_client->next_client;
				}

				//update pointer value
				if(client == NULL)
					break;

			}
			else
			{
				//Next client
				last_client = client;
				client = client->next_client;
			}
		}
}
