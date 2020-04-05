
#ifndef SDHCP_RSA
#define SDHCP_RSA 1

#include <openssl/rsa.h>
#include <openssl/engine.h>
#include <openssl/sha.h>
#include <openssl/bn.h>

// RSA callback function /-\|/-\|
void RSA_callback (int a, int b, void * arg);
int RSA_init_key (RSA * rsa);

#endif

