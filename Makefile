# differents types de fichiers
.SUFFIXES:.o.c.ln


################################
# INITIALISATION DES VARIABLES #
################################

CC=gcc
CFLAGS= -g
LIBS=-lssl

# Les fichiers sources de l'application

COMMON=protocol.c raw.c udp.c config.c rsa.c timeout.c   packet.c database.c common.c pidfile.c signalpipe.c 
SERVER=sdhcpd.c 
CLIENT=sdhcp.c 
#ENTETES=sdhcp.h packet.h


##########
# CIBLES #
##########

all: server client 

server: $(SERVER:.c=.o) $(COMMON:.c=.o) 
	$(CC) -o sdhcpd $(SERVER:.c=.o) $(COMMON:.c=.o) $(LIBS)

client: $(CLIENT:.c=.o) $(COMMON:.c=.o) 
	$(CC) -o sdhcp $(CLIENT:.c=.o) $(COMMON:.c=.o) $(LIBS)

.c.o:
	$(CC) -c $< -o $@
