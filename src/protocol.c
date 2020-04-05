
#include "sdhcp.h"
#include "packet.h"
#include "protocol.h"
#include "rsa.h"
#include "common.h"

/* Super function thanks to patrice laidet*/
int inet_mtos (char * mac_string, char * mac_char)
{
    return sprintf(mac_string,"%02X:%02X:%02X:%02X:%02X:%02X", mac_char[0], mac_char[1], mac_char[2], mac_char[3], mac_char[4], mac_char[5]);
}
// Etape 1 Client
//
// Envoie : Kc
//
int broadcast_sDHCP (struct client_t * client, struct server_t * server)
{
	struct packet_t packet;
	
    char * tmp_buffer;

    printf( "Recherche du serveur SDHCP...\n" );

    bzero( packet.data, BUFFER_SIZE );

    // Packet type 1
    packet_init( packet.data, PACKET_1, 0, 0 );
    
    // KC->N
    tmp_buffer = BN_bn2hex(client->rsa->n );
    packet_add( packet.data, PACKET_OPT_RSA_N, strlen(tmp_buffer), tmp_buffer );
    free(tmp_buffer);
    
    // KC->E
    tmp_buffer = BN_bn2hex( client->rsa->e );
    packet_add( packet.data, PACKET_OPT_RSA_E, strlen(tmp_buffer), tmp_buffer );
    free(tmp_buffer);

    packet.sz = packet_close( packet.data );

    //send udp packet
    send_udp_socket(server->udp, &packet);

}

// Etape 1 Server
//
// Receive: Kc
//
int new_client (struct client_t * client,struct server_t * server, struct packet_t packet)
{
    int optlen;
    char optdata[BUFFER_SIZE];
    static unsigned char idclient = 0;

    if(client == NULL)
    {
        LOG(LOG_CRIT,"unable to create client\n");
        return -ERR_MEMORY;
    }


    client->rsa = RSA_new();
    client->xid = idclient++;
    client->timeout = time(NULL) + LOGIN_TIMEOUT;
    client->n = random();
    //printf("XID: %d\n",client->xid);
    client->state = STATE_CLIENT_DISCOVER;
    strcpy(client->macaddress,"ff:ff:ff:ff:ff:ff");
    strcpy(client->packetmacaddress,"ff:ff:ff:ff:ff:ff");

    //Kc
    packet_get_option( packet->data, PACKET_OPT_RSA_N, &optlen, optdata );
    BN_hex2bn( &(client->rsa->n), optdata );
    packet_get_option( packet->data, PACKET_OPT_RSA_E, &optlen, optdata );
    BN_hex2bn( &(client->rsa->e), optdata );

    //client socket
    /*
     * client->c_sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    memcpy(&client->caddr, &server->caddr, sizeof(server->caddr));
    client->caddr.sin_port = htons( PORT_CLIENT );
*/

    LOG(LOG_INFO, "new client (id=%d, mac=%s, pmac=%s)", client->xid,client->macaddress, client->packetmacaddress);

    /*
    if(client->c_sockfd == -1)
    {
        perror("Error socket");
        return -ERR_SOCKET;
    }
*/
    return 0;
}

/*
int send_packet_2 (struct client_t * client, struct server_t * server)
{
    unsigned char buffer[BUFFER_SIZE];
    char buffer_tmp[BUFFER_SIZE];
    int buffer_tmp_sz;

    char *tmp_buffer;
    unsigned char secnum[16];
    int sz;
    int res;
    int clen = sizeof(client->caddr);

    bzero( buffer, BUFFER_SIZE );
    packet_init( buffer, PACKET_2, 0, client->xid );

    //Ks
    tmp_buffer = BN_bn2hex( server->rsa->n );
    packet_add( buffer, PACKET_OPT_RSA_N, strlen(tmp_buffer), tmp_buffer );
    tmp_buffer = BN_bn2hex( server->rsa->e );
    packet_add( buffer, PACKET_OPT_RSA_E, strlen(tmp_buffer), tmp_buffer );

    //pair(Kc(n))
    sprintf( secnum, "%d", client->n );

    // - RSA Crypt
    client->crypt_kc_n_sz = RSA_public_encrypt( strlen(secnum)+1,
            secnum,
            client->crypt_kc_n,
            client->rsa,
            RSA_PADDING );

    // - Odd data
    buffer_tmp_sz = packet_odd( client->crypt_kc_n, buffer_tmp, client->crypt_kc_n_sz );
    // - Add Odd data to packet
    packet_add( buffer, PACKET_OPT_ODD_EVEN_KNUM, buffer_tmp_sz, buffer_tmp );
    // - Close Packet
    sz = packet_close( buffer );

    //Send Packet
    if( res = sendto( client->c_sockfd, buffer, sz, 0,
                (struct sockaddr *)&client->caddr, clen ) == -1)
    {
        LOG(LOG_ERR, "sendto() failed at packet 2");
        perror("Error sendto");
        return -ERR_SOCKET;
    }
}
// Etape 2
//
// Récéption : Ks, pair(Kc(n))
//
*/
int new_server ( struct client_t * client, struct server_t * server, struct packet_t packet)
{

	char optdata[BUFFER_SIZE];
	char buffer_in_odd[BUFFER_SIZE];
	int optlen;
	
	
    server->xid = packet_id(packet->data, packet->sz);
    printf("Id server: %d\n",server->xid);
    server->rsa = RSA_new();

    // Ks
    packet_get_option( packet->data, 10, &optlen, optdata );
    BN_hex2bn( &(server->rsa->n), optdata );
    packet_get_option( packet->data, 11, &optlen, optdata );
    BN_hex2bn( &(server->rsa->e), optdata );

    // pair(Kc(n))
    packet_get_option( packet->data, 1, &optlen, buffer_in_odd );
}
/*
// Etape 3
//
// Envoie : pair(Ks(m))
//
int send_packet_3 (struct client_t * client, struct server_t * server)
{
    srandom( time(NULL)*3 );
    sec_m = random();
    printf("m = %d\n",sec_m);
    packet_init( buffer, 3, 0, xid );

    sprintf( secnum, "%d", sec_m );
    sz = RSA_public_encrypt( strlen(secnum),
            secnum,
            crypt_buf,
            rsa_s,
            RSA_PADDING );

    // On découpe le paquet crypté en 2 : bits pairs et impairs
    buffer_out_sz = packet_odd( crypt_buf, buffer_out_odd, sz );
    packet_add( buffer, 1, buffer_out_sz, buffer_out_odd );
    buffer_out_sz = packet_even( crypt_buf, buffer_out_even, sz );

    sz = RSA_public_encrypt( strlen("01:02:03:04:05:06"),
            "01:02:03:04:05:06",
            crypt_buf2,
            rsa_s,
            RSA_PADDING );

    packet_add( buffer, PACKET_OPT_KMAC, sz, crypt_buf2 );

    sz = packet_close( buffer );

    saddr.sin_port = htons( PORT_SERVER );
    sendto( s_sockfd, buffer, sz, 0,
            (struct sockaddr *)&saddr, sizeof(saddr) );
}

int rcv_packet_3 ( struct client_t * client, struct server_t * server, char * buffer, int sz )
{
    int optlen;
    unsigned char optdata[BUFFER_SIZE];

    //odd(Ks(m))
    packet_get_option( buffer, PACKET_OPT_ODD_EVEN_KNUM, &optlen, client->odd_ks_m );
    packet_get_option( buffer, PACKET_OPT_KMAC, &optlen, optdata);
    sz = RSA_private_decrypt( optlen,
            optdata,
            client->macaddress,
            server->rsa,
            RSA_PADDING );


    return 0;
}

int send_packet_4 (struct client_t * client, struct server_t * server)
{
    unsigned char buffer[BUFFER_SIZE];
    char buffer_out_even[BUFFER_SIZE];

    int clen = sizeof(client->caddr);
    int buffer_out_sz,sz,res;

    //Init packet
    bzero( buffer, BUFFER_SIZE );
    packet_init( buffer, PACKET_4, 0, client->xid );

    //even(Kc(n))
    buffer_out_sz = packet_even( client->crypt_kc_n, buffer_out_even, client->crypt_kc_n_sz );
    packet_add( buffer, PACKET_OPT_ODD_EVEN_KNUM, buffer_out_sz, buffer_out_even );

    //Close Packet
    sz = packet_close( buffer );

    //Send Packet
    if( res = sendto( client->c_sockfd, buffer, sz, 0,
                (struct sockaddr *)&client->caddr, clen ) == -1)
    {
        LOG(LOG_ERR, "sendto() failed at packet 4");
        perror("Error sendto");
        return -ERR_SOCKET;
    }
    return 0;
}

// Etape 4
//
// Récéption : impair(Kc(n))
//
int rcv_packet_4 ( struct client_t * client, struct server_t * server, char * buffer, int sz )
{

    // impair(Kc(n))
    packet_get_option( buffer, 1, &optlen, buffer_in_even );

    // on regroupe les bits pairs et impairs en un seul paquet
    optlen = packet_collect( buffer_in_odd, buffer_in_even,
            optdata, 128 );

    bzero( crypt_buf, BUFFER_SIZE );
    sz = RSA_private_decrypt( optlen,
            optdata,
            crypt_buf,
            rsa_c,
            RSA_PADDING );

    sec_n = atol( crypt_buf );
}


// Etape 5
//
// Envoie : impair(Ks(m)), Ks(n), pair(Ks(SHA(pass,n,m)))
//
int send_packet_5 (struct client_t * client, struct server_t * server)
{
    printf( "Authentification...\n" );
    packet_init( buffer, 5, 0, xid );

    // impair(Ks(m))
    packet_add( buffer, 1, buffer_out_sz, buffer_out_even );

    // Ks(n)
    sprintf( secnum, "%d", sec_n );
    sz = RSA_public_encrypt( strlen(secnum),
            secnum,
            crypt_buf,
            rsa_s,
            RSA_PADDING );
    packet_add( buffer, 2, sz, crypt_buf );

    // pair(Ks(SHA(pass,n,m)))
    sprintf( optdata, "%s%d%d", argv[2], sec_n, sec_m );
    printf("%s\n",optdata);
    tmp_buffer = SHA1( optdata, strlen(optdata), NULL );
    sz = RSA_public_encrypt( strlen(tmp_buffer),
            tmp_buffer,
            crypt_buf,
            rsa_s,
            RSA_PADDING );
    buffer_out_sz = packet_odd( crypt_buf, buffer_out_odd, sz );
    packet_add( buffer, 3, buffer_out_sz, buffer_out_odd );
    buffer_out_sz = packet_even( crypt_buf, buffer_out_even, sz );

    sz = packet_close( buffer );

    saddr.sin_port = htons( PORT_SERVER );
    sendto( s_sockfd, buffer, sz, 0,
            (struct sockaddr *)&saddr, sizeof(saddr) );


}

int rcv_packet_5 ( struct client_t * client, struct server_t * server, char * buffer, int sz )
{
    char optdata[BUFFER_SIZE];
    char crypt_buf[BUFFER_SIZE];
    char buffer_in_even[BUFFER_SIZE];
    int optlen;

    //Ks(n)
    packet_get_option( buffer, PACKET_OPT_KNUM, &optlen, optdata );
    bzero( crypt_buf, BUFFER_SIZE );
    sz = RSA_private_decrypt( optlen,
            optdata,
            crypt_buf,
            server->rsa,
            RSA_PADDING );

    //printf("%d ?= %d \n",client->n, crypt_buf);
    if( atol(crypt_buf) != client->n )
    {
        LOG(LOG_ERR, "unsafe link (id=%d, mac=%s, pmac=%s)",client->xid,client->macaddress,client->packetmacaddress);
        return -ERR_LINK_UNSAFE;
    }

    //impair(Ks(m))
    packet_get_option( buffer,PACKET_OPT_ODD_EVEN_KNUM, &optlen, buffer_in_even );
    optlen = packet_collect( client->odd_ks_m, buffer_in_even, optdata, 128 );
    bzero( crypt_buf, BUFFER_SIZE );

    sz = RSA_private_decrypt( optlen,
            optdata,
            crypt_buf,
            server->rsa,
            RSA_PADDING );
    client->m = atol(crypt_buf);
    //printf("client->m : %d\n",client->m);
    //pair(Ks(SHA(pass,n,m)))
    packet_get_option( buffer, PACKET_OPT_ODD_EVEN_KSHAPASS, &optlen, client->odd_ks_sha_pass );
    client->odd_ks_sha_pass[optlen] = 0;

    return 0;
}

int send_packet_6 (struct client_t * client, struct server_t * server)
{
    unsigned char buffer[BUFFER_SIZE];
    char crypt_buf[BUFFER_SIZE];
    char secnum[16];
    int sz,res;
    int clen = sizeof(client->caddr);

    bzero( buffer, BUFFER_SIZE );
    packet_init( buffer, PACKET_6, 0, client->xid );

    //Kc(m)
    sprintf( secnum, "%d", client->m );
    sz = RSA_public_encrypt( strlen(secnum),
            secnum,
            crypt_buf,
            client->rsa,
            RSA_PADDING );
    packet_add( buffer, PACKET_OPT_KNUM, sz, crypt_buf );

    //close packet
    sz = packet_close( buffer );

    if( res = sendto( client->c_sockfd, buffer, sz, 0,
                (struct sockaddr *)&client->caddr, clen ) == -1)
    {
        LOG(LOG_ERR, "sendto() failed at packet 6");
        perror("Error sendto");
        return -ERR_SOCKET;
    }


    return 0;
}
// Etape 6

// Récéption : Kc(m)
//
int rcv_packet_6 ( struct client_t * client, struct server_t * server, char * buffer, int sz )
{
    sz = recvfrom( c_sockfd, buffer, BUFFER_SIZE, 0,
            (struct sockaddr *)&saddr, &slen );

    // Kc(m)
    packet_get_option( buffer, 2, &optlen, optdata );
    sz = RSA_private_decrypt( optlen,
            optdata,
            crypt_buf,
            rsa_c,
            RSA_PADDING );

    if( atol(crypt_buf) != sec_m ) {
        fprintf( stderr, "/!\\ \t Laison non sûre, probablement une attaque 'passeur de seau' !!!\n" );
        exit(1);
    }
}


// Etape 7

// Envoie Ks(login), impair(Ks(SHA(pass,n,m)))
//
int send_packet_7 (struct client_t * client, struct server_t * server)
{
    packet_init( buffer, 7, 0, xid );

    // impair(Ks(SHA(pass,n,m))
    packet_add( buffer, 3, buffer_out_sz, buffer_out_even );

    // Ks(login)
    sz = RSA_public_encrypt( strlen(argv[1]),
            argv[1],
            crypt_buf,
            rsa_s,
            RSA_PADDING );
    packet_add( buffer, 4, sz, crypt_buf );

    sz = packet_close( buffer );

    saddr.sin_port = htons( PORT_SERVER );
    sendto( s_sockfd, buffer, sz, 0,
            (struct sockaddr *)&saddr, sizeof(saddr) );

}

int rcv_packet_7 ( struct client_t * client,struct server_t * server, char * buffer, int sz )
{
    char optdata[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    char tmp_buffer[BUFFER_SIZE];
    char crypt_buf[BUFFER_SIZE];
    char buffer_in_even[BUFFER_SIZE];
    int optlen;
    //Ks(login)
    bzero( client->login, LOGIN_MAXSIZE );
    packet_get_option( buffer, PACKET_OPT_KLOGIN, &optlen, optdata );

    sz = RSA_private_decrypt( optlen,
            optdata,
            client->login,
            server->rsa,
            RSA_PADDING );

    //impair(Ks(SHA(pass,n,m))
    packet_get_option( buffer, PACKET_OPT_ODD_EVEN_KSHAPASS, &optlen, buffer_in_even );
    optlen = packet_collect( client->odd_ks_sha_pass, buffer_in_even,
            optdata, 128 );
    bzero( crypt_buf, BUFFER_SIZE );
    sz = RSA_private_decrypt( optlen,
            optdata,
            crypt_buf,
            server->rsa,
            RSA_PADDING );
    get_password( client->login, client->password );
    bzero( buffer2, BUFFER_SIZE );
    sprintf( buffer2, "%s%d%d", client->password, client->n, client->m );
    //    printf("%s\n",buffer2);
    SHA1( buffer2, strlen(buffer2), tmp_buffer );
    if( memcmp( crypt_buf, tmp_buffer, SHA_SIZE ) != 0 ) {
        LOG(LOG_ERR, "login failed (id=%d, login=%s, mac=%s, pmac=%s)", client->xid, client->login, client->macaddress, client->packetmacaddress);
        return -ERR_LOGIN_FAILED;
    } else {
        LOG(LOG_INFO, "login success (id=%d, login=%s, mac=%s, pmac=%s)", client->xid, client->login, client->macaddress, client->packetmacaddress);
    }

    return 0;
}

int send_packet_8 (struct client_t * client, struct server_t * server)
{
    unsigned char buffer[BUFFER_SIZE];
    unsigned char buffer2[BUFFER_SIZE];
    char tmp_buffer[BUFFER_SIZE];
    char optdata[BUFFER_SIZE];
    char crypt_buf[BUFFER_SIZE];
    char secnum[16];
    int sz,res;
    int clen = sizeof(client->caddr);

    bzero( buffer, BUFFER_SIZE );
    packet_init( buffer, PACKET_8, 0, client->xid );

    //Kc(SHA(pass,n,m,config))
    //Get client configuration
    get_config( client->login, client->config );
    bzero( buffer2, BUFFER_SIZE );
    sprintf( buffer2, "%s%d%d%s", client->password, client->n, client->m, client->config );
    //    printf("%s\n",buffer2);
    SHA1( buffer2, strlen(buffer2), optdata );

    sz = RSA_public_encrypt( strlen(optdata),
            optdata,
            crypt_buf,
            client->rsa,
            RSA_PADDING );
    packet_add( buffer, PACKET_OPT_KSHAPASSCONFIG, sz, crypt_buf );

    //Kc(config)
    sz = RSA_public_encrypt( strlen(client->config),
            client->config,
            crypt_buf,
            client->rsa,
            RSA_PADDING );
    packet_add( buffer, PACKET_OPT_KCONFIG, sz, crypt_buf );

    //Close Packet
    sz = packet_close( buffer );

    LOG(LOG_INFO, "send config (id=%d, login=%s, mac=%s, pmac=%s, config=%s)",client->xid, client->login, client->macaddress, client->packetmacaddress, client->config);

    if( res = sendto( client->c_sockfd, buffer, sz, 0,
                (struct sockaddr *)&client->caddr, clen ) == -1)
    {
        LOG(LOG_ERR, "sendto() failed at packet 8");
        perror("sendto");
        return -ERR_SOCKET;
    }
    return 0;
}

// Etape 8
//
// Récéption : Kc(SHA(pass,n,m,config)), Kc(config)
//
int rcv_packet_8 ( struct client_t * client, struct server_t * server, char * buffer, int sz )
{
    // Kc(config)
    bzero( config, BUFFER_SIZE );
    packet_get_option( buffer, 6, &optlen, optdata );
    sz = RSA_private_decrypt( optlen,
            optdata,
            config,
            rsa_c,
            RSA_PADDING );
    printf( "Configuration : %s\n", config );

    // Kc(SHA(pass,n,m,config))
    sprintf( optdata, "%s%d%d%s", argv[2], sec_n, sec_m, config );
    tmp_buffer = SHA1( optdata, strlen(optdata), NULL );
    packet_get_option( buffer, 5, &optlen, optdata );
    sz = RSA_private_decrypt( optlen,
            optdata,
            crypt_buf,
            rsa_c,
            RSA_PADDING );

    for(i=0;i<SHA_SIZE;i++)
    {
        if(tmp_buffer[i] != crypt_buf[i])
            printf("==>> %d\n",i);
    }

    if( memcmp( tmp_buffer, crypt_buf, SHA_SIZE ) != 0 ) {
        printf( "/!\\ Serveur non autorisé ou authentification incorrecte, opération annulée. \n" );
        exit(1);
    }

    printf( "Client configuré.\n" );

}
*/

