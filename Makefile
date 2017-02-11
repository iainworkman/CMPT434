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

emulator: emulator$(ARCH).o
	$(CC) $(CFLAGS) -o $@ emulator$(ARCH).o

emulator$(ARCH).o: emulator.c
	$(CC) $(CFLAGS) -c emulator.c -o $@

.PHONY: clean


clean:
	rm -rf *.o *.a $(TARGETS) *~
