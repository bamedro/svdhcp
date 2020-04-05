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

#ifndef __PACKET_H__
#define __PACKET_H__

#define PACKET_1 1 //C->S Kc
#define PACKET_2 2 //S->C
#define PACKET_3 3
#define PACKET_4 4
#define PACKET_5 5
#define PACKET_6 6
#define PACKET_7 7
#define PACKET_8 8
#define PACKET_8 8

#define PACKET_OPT_ODD_EVEN_KNUM 1
#define PACKET_OPT_KNUM 2
#define PACKET_OPT_ODD_EVEN_KSHAPASS 3
#define PACKET_OPT_KLOGIN 4
#define PACKET_OPT_KSHAPASSCONFIG 5
#define PACKET_OPT_KCONFIG 6
#define PACKET_OPT_KMAC 7
#define PACKET_OPT_RSA_N 10
#define PACKET_OPT_RSA_E 11

#define PACKET_POS_OPT_START 6
enum pkt_option_enum { NORMAL, ODD, EVEN };

typedef enum pkt_option_enum padding_e;

struct packet_t {
	char data [ MAX_DATA_LENGTH ];
	int sz;
};

int packet_init ( struct packet_t * packet, char protocol, char type, unsigned int xid );
int packet_get ( struct packet_t * packet, char id, char * data, int sz, padding_e padding );
int packet_add ( struct packet_t * packet, char id, char * data, int sz, padding_e padding );
int packet_close ( struct packet_t * packet );

void packet_sniffer ( struct packet_t * packet );

int packet_protocol ( struct packet_t packet );
int packet_type ( struct packet_t * packet );
int packet_xid ( struct packet_t packet );

#endif
