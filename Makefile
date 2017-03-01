####################################################
# Name: Iain Workman
# NSID: ipw969
# Student No.: 11139430
####################################################

CC=gcc
CFLAGS=-Wall -pedantic -g

#define the system architecture.
OS=$(shell uname -s)
ARCH=_$(OS)$(UNAME_M)

TARGETS=emulator sr_client sr_server

all: $(TARGETS)

# Part A Build - Lossy UPD Emulator
####################################

emulator: emulator$(ARCH).o libList$(ARCH).a
	$(CC) $(CFLAGS) -I. -L. -o $@ emulator$(ARCH).o libList$(ARCH).a -lList$(ARCH) -lrt

emulator$(ARCH).o: emulator.c
	$(CC) $(CFLAGS) -c emulator.c -o $@

# Part B Build - Selective Repeat Client & Server
###################################################

sr_client: sr_client$(ARCH).o
	$(CC) $(CFLAGS) -I. -L. -o $@ sr_client$(ARCH).o -lpthread

sr_client$(ARCH).o: sr_client.c
	$(CC) $(CFLAGS) -c sr_client.c -o $@

sr_server: sr_server$(ARCH).o 
	$(CC) $(CFLAGS) -I. -L. -o $@ sr_server$(ARCH).o -lpthread

sr_server$(ARCH).o: sr_server.c
	$(CC) $(CFLAGS) -c sr_server.c -o $@

# List Library
########################################
libList$(ARCH).a: list_adders$(ARCH).o list_movers$(ARCH).o \
									list_removers$(ARCH).o
	ar rcs libList$(ARCH).a list_adders$(ARCH).o list_movers$(ARCH).o \
										list_removers$(ARCH).o

list_adders$(ARCH).o: list_adders.c
	$(CC) $(CFLAGS) -c list_adders.c -o $@

list_movers$(ARCH).o: list_movers.c
	$(CC) $(CFLAGS) -c list_movers.c -o $@

list_removers$(ARCH).o: list_removers.c
	$(CC) $(CFLAGS) -c list_removers.c -o $@

.PHONY: clean


clean:
	rm -rf *.o *.a $(TARGETS) *~
