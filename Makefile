# differents types de fichiers
.SUFFIXES:.o.c.ln


################################
# INITIALISATION DES VARIABLES #
################################

CC=gcc
CFLAGS= -g
LIBS=-lssl

# Les fichiers sources de l'application

COMMON=src/protocol.c src/raw.c src/udp.c src/config.c src/rsa.c src/timeout.c   src/packet.c src/database.c src/common.c src/pidfile.c src/signalpipe.c
SERVER=src/svdhcpd.c
CLIENT=src/svdhcp.c
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
