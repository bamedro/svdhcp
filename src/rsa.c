#include <stdlib.h>

#include <openssl/rsa.h>
//#include <openssl/engine.h>
//#include <openssl/sha.h>
//#include <openssl/bn.h>
#include <syslog.h>

#include "svdhcp.h"
#include "rsa.h"
#include "common.h"

/* RSA Callback function */
void RSA_callback (int a, int b, void * arg)
{
    static char list[]= "\\|/-\\|/-";
    static char n=0;

    printf("\b%c",list[(n++)%(sizeof(list)-1)]);
    fflush(stdout);
}
int RSA_init_key (RSA * rsa)
{
    /* Server RSA Key */
    LOG(LOG_INFO,"Generating %d bits RSA key...", RSAKEY_SIZE );
    fflush(stdout);
    rsa = RSA_generate_key( RSAKEY_SIZE, RSA_F4, RSA_callback, NULL );
    printf("\b");
    LOG(LOG_INFO,"generation of %d bits RSA bey successfull",RSAKEY_SIZE);

    return 1;
}
