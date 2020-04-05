#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>

#define NB 1000000

int main( void ) {
  int debut, fin;
  int i;
  char msg[30];
  char md[25];


  printf( "Test 1 : Calcule des SHA1...\n" );
  debut = time(NULL);
  for( i = 0 ; i<NB ; i++ ) {
    sprintf( msg, "%dco%ducou%d", i );
    SHA1( msg, strlen(msg), NULL );
  }
  fin = time(NULL);
  printf( "...effectuée : %d secondes.\n\n",
	  fin - debut );


  printf( "Test 1 : Calcule des SHA1...\n" );
  debut = time(NULL);
  for( i = 0 ; i<NB ; i++ ) {
    sprintf( msg, "%dco%ducou%d", i );
    SHA1( msg, strlen(msg), md );
  }
  fin = time(NULL);
  printf( "...effectuée : %d secondes.\n\n",
	  fin - debut );


  return 1;
}
