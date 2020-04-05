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

#include "sdhcp.h"
#include "rsa.h"
#include "protocol.h"
#include "raw.h"
#include "udp.h"
#include "packet.h"
#include "common.h"
#include "signalpipe.h"
#include "timeout.h"
#include "config.h" 
// Function executed when receive data
int reply_to_server ( struct client_t * client);
// Timeout checking function


	int 
main( int argc, char **argv )
{
	fd_set rfds;
	struct timeval tv;
	int retval;
	
	int maxsock;
	int res;

	struct client_t client;
	struct packet_t packet;
	

	srandom( time(NULL) );

	// --- First lines printed 
	printf( "svDHCP - Secure authentification client - %s\n",VERSION );
	printf( "Visit http://www.svdhcp.org\n");

	// --- Sigterm setup 
	client.sig_sockfd = sig_setup();

	// --- Loading config file
	res = config_load_client(&client);
	if( res < 0 ) return(EXIT_FAILURE);

	// --- Init sockets
	res = init_raw_socket(&client.raw_tx,"eth0",10,20);
	if( res < 0 ) return(EXIT_FAILURE);
	//if( server.raw.iphead->saddr == inet_addr("255.255.255.255")) return (EXIT_FAILURE);
	res = init_udp_socket(&client.udp_rx);
	if( res < 0 ) return(EXIT_FAILURE);
	
	// --- Initialize client list
	client.list_server = NULL;


	// --- Reception Loop 
	LOG(LOG_INFO,"waiting servers ...",RSAKEY_SIZE);
	while( 1 ) {
		// Udp Socket lost
		while(client.udp_rx.sockfd < 0)
		{
			LOG(LOG_ERR,"udp socket lost, trying to restore ...");
			res = init_udp_socket(&client.udp_rx);
			if(res) bind_udp_socket(&client.udp_rx, PORT_CLIENT);
			sleep(1);
		}

		// Raw Socket lost
		while(client.raw_tx.sockfd < 0)
		{
			LOG(LOG_ERR,"raw socket lost, trying to restore ...");
			init_raw_socket(&client.raw_tx,"eth0",10,20);
			sleep(1);
		}

		// Add sockets to select list
		FD_ZERO(&rfds);
		FD_SET(client.udp_rx.sockfd, &rfds);
		FD_SET(client.sig_sockfd, &rfds);
		maxsock = ( (client.sig_sockfd > client.udp_rx.sockfd) ? client.sig_sockfd : client.udp_rx.sockfd );

		// Timeout Loop
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		// Looking for a new packet
		retval = select(maxsock+1, &rfds, NULL, NULL, &tv);

		if(retval)
		{
			// if sigterm
			if(FD_ISSET(client.sig_sockfd,&rfds))
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
			if(FD_ISSET(client.udp.sockfd,&rfds))
			{
				//Read udp packet
				recv_udp_socket (&(client.udp), &packet);
				// Reply to the client
				reply_to_server(&client, &packet);
			}

		}
		// Check for timeout
		timeout_check_server(&client);
	}


	exit(EXIT_SUCCESS);
}


// Function executed when receive data
int reply_to_server ( struct client_t * client, struct packet_t * packet)
{
	struct server_t * server;
	int res, type, id;

	DEBUG(LOG_INFO, "New packet received (%d bytes)", packet->sz);

	if((type = packet_type(packet->data, packet->sz)) == -1)
		return -ERR_WRONG_PACKET;

	id = packet_id(packet->data,packet->sz);
	DEBUG(LOG_INFO," -> type: %d id: %d",type,id );
	//packet_sniffer(buffer);

	// Case of first packet
	if(type != PACKET_2)
	{
		// Looking for client id in list
		server = client->list_server;
		while(server->xid != id)
		{
			printf("xid = %d\n",server->xid);
			//End of list
			if(server->next_server == NULL)
			{
				printf("Packet id not found\n");
				return -ERR_WRONG_PACKET;
			}
			//Next client
			server = server->next_server;
		}

		// Look if it is the expected packet
		if(type != server->expected_packet)
		{
			printf("Wrong Packet\n");
			return -ERR_WRONG_PACKET;
		}
	}


	switch(type)
	{
		case PACKET_2 :
			//Etape 1
			//Receive :  Kc
			DEBUG(LOG_INFO,"Step 1 : Get Client RSA Key (Kc)" );
			server = (struct server_t *) malloc(sizeof(struct server_t));
			server->next_server = client->list_server;
			client->list_server = server;
			if((res = new_server(client, server, packet)) < 0)
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




