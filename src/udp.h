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
#ifndef SDHCP_UDP
#define SDHCP_UDP 1

#include "packet.h"

struct udp_rx_t {
	struct sockaddr_in saddr;
};

struct udp_tx_t {
	struct sockaddr_in saddr;
};

struct udp_t {
	int sockfd;
	struct udp_rx_t rx;
	struct udp_tx_t tx;
};


// Public
int init_udp_socket ( struct udp_t * udp );
int bind_udp_socket ( struct udp_t * udp, int port );
int recv_udp_socket ( struct udp_t * udp, struct packet_t * packet);
int send_udp_socket ( struct udp_t * udp, struct packet_t * packet);

// Private
int open_udp_socket ( struct udp_t * udp );
#endif
