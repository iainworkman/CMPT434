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

TARGETS=dtn_simulation

all: $(TARGETS)

# Test
####################################

test: test$(ARCH).o network$(ARCH).o
	$(CC) $(CFLAGS) -I. -L. -o $@ $^ -lpthread

test$(ARCH).o: 
	$(CC) $(CFLAGS) -c test.c -o $@

# DTN Build - Delay Tolerant Network
####################################
dtn_simulation: dtn_simulation$(ARCH).o dtn_grid$(ARCH).o dtn_node$(ARCH).o \
								network$(ARCH).o
	$(CC) $(CFLAGS) -I. -L. -o $@ $^ -lpthread -lm

dtn_simulation$(ARCH).o: dtn_simulation.c
	$(CC) $(CFLAGS) -c dtn_simulation.c -o $@

dtn_node$(ARCH).o: dtn_node.c
	$(CC) $(CFLAGS) -c dtn_node.c -o $@

dtn_grid$(ARCH).o: dtn_grid.c
	$(CC) $(CFLAGS) -c dtn_grid.c -o $@

network$(ARCH).o: network.c
	$(CC) $(CFLAGS) -c network.c -o $@

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
	rm -rf *.o *.a $(TARGETS) test *~
