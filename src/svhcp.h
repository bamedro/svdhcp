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

#ifndef SDHCP_HEADER
#define SDHCP_HEADER 1

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "common.h" 

#define VERSION                 "1.0.0"
#define SDHCP_DEBUG             1

#define LOGIN_MAXSIZE	50
#define MAX_DATA_LENGTH 4048

#define ERR_SOCKET		1
#define ERR_TOO_MANY_CLIENT 	2
#define ERR_WRONG_PACKET	3
#define ERR_LINK_UNSAFE		4
#define ERR_LOGIN_FAILED	5
#define ERR_MEMORY              6
#define ERR_PACKET_OPT_NOT_FOUND 7

#define STATE_CLIENT_DISCOVER 1


#define PORT_SERVER	17004
#define PORT_CLIENT	17005
#define BUFFER_SIZE	1024

#define RSAKEY_SIZE	1024
#define RSA_PADDING	RSA_PKCS1_OAEP_PADDING
#define SHA_SIZE	20

#define LOGIN_TIMEOUT   2

#endif
