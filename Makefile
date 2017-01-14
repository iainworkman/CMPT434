####################################################
# Name: Iain Workman
# NSID: ipw969
# Student No.: 11139430
####################################################

CC=gcc
CFLAGS=-Wall -pedantic -g

UNAME_M=$(shell uname -m)

ARCH=_$(UNAME_M)

all: serv_select serv_proc serv_thread client
tests: list_test

####################
# Tests
####################

list_test: list_test.o libCalendar$(ARCH).a libList$(ARCH).a
	$(CC) $(CFLAGS) -I. -L. -o $@ $^ -lList$(ARCH) -lCalendar$(ARCH)

list_test.o: list_test.c
	$(CC) $(CFLAGS) -c list_test.c -o $@
####################
# Proc Server
####################

serv_proc: serv_proc.o libCalendar$(ARCH).a libList$(ARCH).a
	$(CC) $(CFLAGS) -I. -L. -o $@ $^ -lList$(ARCH) -lCalendar$(ARCH)

serv_proc.o: serv_proc.c
	$(CC) $(CFLAGS) -c serv_proc.c -o $@

####################
# Threaded Server
####################

serv_thread: serv_thread.o libCalendar$(ARCH).a libList$(ARCH).a
	$(CC) $(CFLAGS) -I. -L. -o $@ $^ -lList$(ARCH) -lCalendar$(ARCH)

serv_thread.o: serv_thread.c
	$(CC) $(CFLAGS) -c serv_thread.c -o $@

####################
# Select Server
####################

serv_select: serv_select.o libCalendar$(ARCH).a libList$(ARCH).a 
	$(CC) $(CFLAGS)	-I. -L. -o $@ $^ -lList$(ARCH) -lCalendar$(ARCH)

serv_select.o: serv_select.c
	$(CC) $(CFLAGS) -c serv_select.c -o $@

####################
# Client
####################

client: client.c
	$(CC) $(CFLAGS) client.c -o $@

####################
# Calendar
####################

libCalendar$(ARCH).a: calendar.o
	ar rcs $@ $^

calendar$(ARCH).o: calendar.c
	$(CC) $(CFLAGS) -c calendar.c -o $@

####################
# List
####################

libList$(ARCH).a: 	list_adders$(ARCH).o \
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

