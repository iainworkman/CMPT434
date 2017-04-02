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

# DTN Build - Delay Tolerant Network
####################################
dtn_simulation: dtn_simulation$(ARCH).o dtn$(ARCH).o libList$(ARCH).a
	$(CC) $(CFLAGS) -I. -L. -o $@ $^ -lList$(ARCH) -lm

dtn_simulation$(ARCH).o: dtn_simulation.c
	$(CC) $(CFLAGS) -c dtn_simulation.c -o $@

dtn$(ARCH).o: dtn.c
	$(CC) $(CFLAGS) -c dtn.c -o $@

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
