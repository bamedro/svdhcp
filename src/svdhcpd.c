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

#include "svdhcp.h"
#include "rsa.h"
#include "protocol.h"
#include "raw.h"
#include "udp.h"
#include "packet.h"
#include "common.h"
#include "signalpipe.h"
#include "timeout.h"
#include "config.h"
#include <unistd.h>

// Function executed when receive data
//int reply_to_client ( struct server_t * server);
int reply_to_client ( struct server_t * server, struct packet_t * packet);
// Timeout checking function


	int
main( int argc, char **argv )
{
	fd_set rfds;
	struct timeval tv;
	int retval;

	int maxsock;
	int res;

	struct server_t server;
	struct packet_t packet;

	srandom( time(NULL) );

	// --- First lines printed
	printf( "sDHCPd - Secure authentification IP server - %s\n",VERSION );
	printf( "Visit http://sdhcpd.ipxwarzone.com\n");

	// --- Sigterm setup
	server.sig_sockfd = sig_setup();

	// --- Loading config file
	res = config_load_server(&server);
	if( res < 0 ) return(EXIT_FAILURE);

	// --- Init sockets
	res = init_raw_socket(&server.raw_tx,"eth0",10,20);
	if( res < 0 ) return(EXIT_FAILURE);
	//if( server.raw.iphead->saddr == inet_addr("255.255.255.255")) return (EXIT_FAILURE);
	res = init_udp_socket(&server.udp_rx);
	if( res < 0 ) return(EXIT_FAILURE);

	// --- Initialize client list
	server.list_client = NULL;


	// --- Reception Loop
	LOG(LOG_INFO,"waiting clients ...",RSAKEY_SIZE);
	while( 1 ) {
		// Udp Socket lost
		while(server.udp_rx.sockfd < 0)
		{
			LOG(LOG_ERR,"udp socket lost, trying to restore ...");
			res = init_udp_socket(&server.udp_rx);
			if(res) bind_udp_socket(&server.udp_rx, PORT_SERVER);
			sleep(1);
		}

		// Raw Socket lost
		while(server.raw_tx.sockfd < 0)
		{
			LOG(LOG_ERR,"raw socket lost, trying to restore ...");
			init_raw_socket(&server.raw_tx,"eth0",10,20);
			sleep(1);
		}

		// Add sockets to select list
		FD_ZERO(&rfds);
		FD_SET(server.udp_rx.sockfd, &rfds);
		FD_SET(server.sig_sockfd, &rfds);
		maxsock = ( (server.sig_sockfd > server.udp_rx.sockfd) ? server.sig_sockfd : server.udp_rx.sockfd );

		// Timeout Loop
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		// Looking for a new packet
		retval = select(maxsock+1, &rfds, NULL, NULL, &tv);

		if(retval)
		{
			// if sigterm
			if(FD_ISSET(server.sig_sockfd,&rfds))
			{
				DEBUG(LOG_INFO,"signal received\n");
				switch(sig_read())
				{
					case SIGUSR1:
						LOG(LOG_INFO,"SIGUSR1 received");
						break;
					case SIGUSR2:
						LOG(LOG_INFO,"SIGUSR2 received");
						break;
					case SIGTERM:
						LOG(LOG_INFO,"SIGTERM received");
						exit(EXIT_SUCCESS);
						break;
				}
			}
			// if udp packet
			if(FD_ISSET(server.udp_rx.sockfd,&rfds))
			{
				//Read udp packet
				recv_udp_socket (&(server.udp_rx),&packet);
				// Reply to the client
				reply_to_client(&server,&packet);
			}

		}
		// Check for timeout
		timeout_check_client(&server);
	}


	exit(EXIT_SUCCESS);
}


// Function executed when receive data
int reply_to_client ( struct server_t * server, struct packet_t * packet)
{
	struct client_t * client;
	int res, type, id;

	DEBUG(LOG_INFO, "New packet received (%d bytes)" ,packet->sz);

	if((type = packet_type(packet)) == -1)
		return -ERR_WRONG_PACKET;

	id = packet_id(packet);
	DEBUG(LOG_INFO," -> type: %d id: %d",type,id );
	//packet_sniffer(buffer);

	// Case of first packet
	if(type != PACKET_1)
	{
		// Looking for client id in list
		client = server->list_client;
		while(client->xid != id)
		{
			printf("xid = %d\n",client->xid);
			//End of list
			if(client->next_client == NULL)
			{
				printf("Packet id not found\n");
				return -ERR_WRONG_PACKET;
			}
			//Next client
			client = client->next_client;
		}

		// Look if it is the expected packet
		if(type != client->expected_packet)
		{
			printf("Wrong Packet\n");
			return -ERR_WRONG_PACKET;
		}
	}


	switch(type)
	{
		case PACKET_1 :
			//Etape 1
			//Receive :  Kc
			DEBUG(LOG_INFO,"Step 1 : Get Client RSA Key (Kc)" );
			client = (struct client_t *) malloc(sizeof(struct client_t));
			client->next_client = server->list_client;
			server->list_client = client;
			if((res = new_client(client, server, packet)) < 0)
				return res;
		/*
			//Step 2
			//Sending : Ks, odd(Kc(n))
			DEBUG(LOG_INFO,"Step 2 : Send Server RSA Key (Ks) and pair(Kc(n))" );
			if((res = send_packet_2(client, server)) < 0)
				return res;

			client->expected_packet = PACKET_3;
*/
			break;
/*
			   case PACKET_3:
			//Step 3
			//Receiving : odd(Ks(m))
			DEBUG(LOG_INFO,"Step 3 : Receive odd(Ks(m))" );
			if((res = rcv_packet_3(client, server,  buffer, sz))<0)
			return res;
			//Step 4
			//Sending : even(Kc(n))
			DEBUG(LOG_INFO,"Step 4 : Send even(Kc(n))" );
			if((res = send_packet_4(client, server)) < 0)
			return res;
			client->expected_packet = PACKET_5;


			break;
			case PACKET_5:
			//Step 5
			//Receiving : even(Ks(m)), Ks(n), odd(Ks(SHA(pass,n,m)))
			DEBUG(LOG_INFO,"Step 5 : Receive even(Ks(m)), Ks(n), odd(Ks(SHA(pass,n,m)))" );
			if((res = rcv_packet_5(client, server,  buffer, sz))<0)
			return res;

			//Step 6
			//Sending : Kc(m)
			DEBUG(LOG_INFO, "Step 6 : Send Kc(m)" );
			if((res = send_packet_6(client, server)) < 0)
			return res;
			client->expected_packet = PACKET_7;

			break;
			case PACKET_7:
			//Step 7
			//Receiving : Ks(login), even(Ks(SHA(pass,n,m)))
			DEBUG(LOG_INFO, "Step 7 : Receive Ks(login), even(Ks(SHA(pass,n,m)))" );
			if((res = rcv_packet_7(client, server,  buffer, sz))<0)
			return res;

			//Step 8
			//Sending : Kc(SHA(pass,n,m,config)), Kc(config)
			DEBUG(LOG_INFO, "Step 8 : Send Kc(SHA(pass,n,m,config)), Kc(config)" );
			if((res = send_packet_8(client, server)) < 0)
			return res;

			client->expected_packet = -1;

			break;
			*/
		default:
			DEBUG(LOG_ERR,"wrong packet ...");
			return -ERR_WRONG_PACKET;
			break;
	}

	return 1;
}
