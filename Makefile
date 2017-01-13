####################################################
# Name: Iain Workman
# NSID: ipw969
# Student No.: 11139430
####################################################

CC=gcc
CFLAGS=-Wall -pedantic -g

UNAME_M=$(shell uname -m)

ARCH=_$(UNAME_M)

all: list libcalendar$(ARCH).a

####################
# Calendar
####################

libcalendar$(ARCH).a: calendar.o
	ar rcs $@ $^

calendar$(ARCH).o: calendar.c
	$(CC) $(CFLAGS) -c calendar.c -o $@

####################
# List
####################
list: liblist$(ARCH).a

liblist$(ARCH).a: 	list_adders$(ARCH).o \
										list_movers$(ARCH).o \
									 	list_removers$(ARCH).o
	ar rcs $@ $^ 

list_adders$(ARCH).o: list_adders.c
	$(CC) $(CFLAGS) -c list_adders.c -o $@

list_movers$(ARCH).o: list_movers.c
	$(CC) $(CFLAGS) -c list_movers.c -o $@

list_removers$(ARCH).o: list_removers.c
	$(CC) $(CFLAGS) -c list_removers.c -o $@

.PHONY: clean

clean:
	rm -rf *.o *.a

