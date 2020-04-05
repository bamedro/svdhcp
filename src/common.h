/* common.h
 *
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
#ifndef SDHCP_COMMON
#define SDHCP_COMMON 1

long uptime(void);
int pidfile_acquire(const char *pidfile);
void pidfile_write_release(int pid_fd);
void background(const char *pidfile);
void start_log_and_pid(const char *client_server, const char *pidfile);
void sdhcp_logging(int level, const char *fmt, ...);


#define LOG(level, str, args...) sdhcp_logging(level, str, ## args)

#ifdef SDHCP_DEBUG
# define DEBUG(level, str, args...) LOG(level, str, ## args)
#else
# define DEBUG(level, str, args...) do {;} while(0)
#endif

#endif

