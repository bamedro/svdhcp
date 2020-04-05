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




#include "packet.h"
#include "svdhcp.h"

static int packet_pos;


// Packet parser
	int
packet_get ( struct packet_t * packet, char id, char * data, int sz, padding_e padding)
{
	int len, i, j;

	i = PACKET_POS_OPT_START;

	// Loop seeking an option
	while( packet->data[i] != id ) {

		// end packet Test
		if( packet->data[i] == 255 )
		{
	    		LOG(LOG_ERR,"packet get option - option %d not found", id);
			return -ERR_PACKET_OPT_NOT_FOUND;
		}

		// calculate size of option
		len = packet->data[i+1] + packet->data[i+2]*0x100;

		// next field
		i += len + 3;

		// over memory test
		if( (i+2) >= PACKET_MAX_DATA_SIZE )
		{
	    		LOG(LOG_ERR,"packet get option - wrong packet", id);
			return -ERR_WRONG_PACKET;
		}
	}

	// calculate size of option
	len = packet[curr+1]*256 + packet[curr+2];

	// test option size
	if ( (len != sz) && (sz!=0) )
	{
	    	LOG(LOG_ERR,"packet get option - wrong size option", id);
		return -ERR_PACKET_OPT_WRONG_SIZE;
	}

	// copy result witch padding
	switch (padding)
	{
		case ODD :
			j = 0;
			while (j < len)
			{
				data[j<<1] = packet->data[i+3+j];
				j++;
			}
			break;
		case EVEN :
			j = 0;
			while (j < len)
			{
				data[j<<1+1] = packet->data[i+3+j];
				j++;
			}
			break;
		case NORMAL :
			memcpy( data, &packet->data[i+3], len );
			break;
	}
	return 1;
}

// Initialize packet with types
int
packet_init ( struct packet_t * packet, char protocol, char type, unsigned int xid );
{
	packet->data[0] = protocol;	// op
	packet->data[1] = type;	// flag
	packet->data[2] = xid&0x000000FF;	// xid
	packet->data[3] = xid&0x0000FF00;	// xid
	packet->data[4] = xid&0x00FF0000;	// xid
	packet->data[5] = xid&0xFF000000;	// xid

	packet->sz = PACKET_POS_OPT_START;

	return 1;
}


// Add field into packet
int
packet_add ( struct packet_t * packet, char id, char * data, short sz, padding_e padding );
{
	int i;
	int opt_sz = 0;
	char * opt_data;

	char buffer [PACKET_MAX_OPT_LENGTH/2];

	if ( sz > PACKET_MAX_OPT_LENGTH )
	{
	    	LOG(LOG_ERR,"packet add option - option size is too large", id);
		return -ERR_PACKET_OPT_TOO_LARGE;
	}

	switch(padding)
	{
		case ODD:
			i=0;
			while (i < sz)
			{
				buffer[i>>1]=data[i];
				i += 2;
				opt_sz++;
			}
			opt_data = buffer;
			break;
		case EVEN:
			i=1;
			while (i < sz)
			{
				buffer[i>>1]=data[i];
				i += 2;
				opt_sz++;
			}
			opt_data = buffer;
			break;

		case NORMAL:
			opt_sz = sz;
			opt_data = data;

			break;
	}

	packet[packet->sz++] = id;
	packet[packet->sz++] = opt_sz&0x00FF;
	packet[packet->sz++] = opt_sz%0xFF00;
	memcpy( &packet->data[packet->sz], opt_data, opt_sz );
	packet->sz += opt_sz;

	return 1;
}

// End Packet Tag
int packet_close( char *packet )
{
	packet->data[packet->sz++] = 255; // Marqueur de fin d'options

	return packet->sz;
}

// Read packet header
	void
packet_sniffer ( struct packet_t * packet )
{
	printf( "\nSNIFFER\n=======\n" );
	printf( "Header : %d / %d / %d\n",
			buffer[0], buffer[1],
			buffer[3]*0x100+buffer[2]);
}

	int
packet_protocol ( struct packet_t * packet )
{
	if( packet->sz >= PACKET_POS_OPT_START )
		return (packet->data[0]);
	else
		return -ERR_WRONG_PACKET;
}
	int
packet_type ( struct packet_t * packet )
{
	if( packet->sz >= PACKET_POS_OPT_START )
		return (packet->data[1]);
	else
		return -ERR_WRONG_PACKET;
}

	int
packet_xid ( struct packet_t * packet )
{
	if( packet->sz >= PACKET_POS_OPT_START)
		return (packet->data[2]+packet->data[3]*0x100);
	else
	{
		return -ERR_WRONG_PACKET;
	}
}
