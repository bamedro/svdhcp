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

/* Deux fonctions qui simulent un accès à la base de données */
int get_password( char *login,
  char *pass )
{
    switch(login[0])
    {
        case 'a':
strcpy( pass, "toto" );
      break;
        case 'b':
strcpy( pass, "titi" );
      break;
        case 'c':
strcpy( pass, "tata" );
      break;
    }
return 1;
}
int get_config( char *login,
  char *config )
{
    switch(login[0])
    {
        case 'a':
      strcpy( config, "IP:192.168.1.1 MASQUE:255.255.255.0" );
      break;
        case 'b':
      strcpy( config, "IP:192.168.1.2 MASQUE:255.255.255.0" );
      break;
        case 'c':
      strcpy( config, "IP:192.168.1.3 MASQUE:255.255.255.0" );
      break;
    }

  return 1;
}
