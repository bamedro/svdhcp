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

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>


#include "signalpipe.h"
#include "common.h"

static int sig_sockfd[2];

static void sig_handler ( int sig )
{
	if (send(sig_sockfd[1], &sig, sizeof(sig), MSG_DONTWAIT) < 0)
		DEBUG(LOG_ERR, "Could not send signal: %m");
}

/* Call this before doing anything else. Sets up the socket pair
 * and installs the signal handler */
int sig_setup ( void )
{
	socketpair(AF_UNIX, SOCK_STREAM, 0, sig_sockfd);
	signal(SIGUSR1, sig_handler);
	signal(SIGUSR2, sig_handler);
	signal(SIGTERM, sig_handler);
        return sig_sockfd[0];
}


int sig_read ( void )
{
	int sig;

	if (read(sig_sockfd[0], &sig, sizeof(sig)) < 0)
		return -1;
	return sig;
}
