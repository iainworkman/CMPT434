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

TARGETS=emulator

all: $(TARGETS)

# Part A Build - Lossy UPD Emulator
####################################

emulator: emulator$(ARCH).o libList$(ARCH).a
	$(CC) $(CFLAGS) -I. -L. -o $@ emulator$(ARCH).o libList$(ARCH).a -lList$(ARCH)

emulator$(ARCH).o: emulator.c
	$(CC) $(CFLAGS) -c emulator.c -o $@

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
