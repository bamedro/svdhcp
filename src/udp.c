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
#include "protocol.h"

int open_udp_socket ( struct udp_t * udp )
{
	int sockfd;
	sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if(sockfd == -1)
	{
		perror("server socket");
		LOG(LOG_CRIT,"Unable to open server socket");
		return -ERR_SOCKET;
	}
	return sockfd;
}

int init_udp_socket ( struct udp_t * udp )
{
	// Sockaddr_in initialization 
	bzero( udp->rx.saddr, sizeof(struct sockaddr_in) );
	bzero( udp->rx.bind_saddr, sizeof(struct sockaddr_in) );
	bzero( udp->tx.saddr, sizeof(struct sockaddr_in) );

	// serveur socket 
	udp->rx.bind_saddr.sin_family = AF_INET;
	udp->rx.bind_saddr.sin_addr.s_addr = INADDR_ANY;
	udp->sockfd = -1;

	udp->sockfd = open_udp_socket(udp);
}

int bind_udp_socket ( struct udp_t * udp, int port)
{
	udp->rx.saddr.sin_port = htons(port);
	if (bind( udp->sockfd, (struct sockaddr *)&(udp->rx.bind_saddr), sizeof(struct sockaddr_in) ) == -1)
	{
		perror("bind server socket");
		LOG(LOG_CRIT,"Unable to bind server port %d", port );
		return -ERR_SOCKET;
	}
	return 1;
}

int send_udp_socket ( struct udp_t * udp, struct packet_t * packet)
{
    int rez;
    
    rez = sendto( udp->sockfd, packet->data, packet->sz, 0,(struct sockaddr *)&udp->tx.saddr, sizeof(struct sockaddr_in) );

    if( rez < 0 )
    {
	    perror("sendto");
	    LOG(LOG_ERR,"udp socket - send data failed");
    }
    
    return rez;
}


int recv_udp_socket ( struct udp_t * udp, struct packet_t * packet )
{
	struct sockaddr_in saddr;
	int clen = sizeof(struct sockaddr_in);

	// Reset packet buffer
	bzero( packet->data, BUFFER_SIZE );
	bzero( &saddr, sizeof(struct sockaddr_in) );

	// Read packet
	packet->sz = recvfrom( udp->sockfd, packet->data, BUFFER_SIZE, MSG_DONTWAIT,
			(struct sockaddr *)&saddr, &clen );

	if(packet->sz == -1)
	{
		perror("recvfrom");
		LOG(LOG_ERR,"receive data from server socket failed");
	}

	return packet->sz;
}


