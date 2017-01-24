/*
 * Name: Iain Workman
 * NSID: ipw969
 * Student No.: 11139430
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>

#include "calendar.h"

pthread_mutex_t calendar_mutex = PTHREAD_MUTEX_INITIALIZER;

void* child_proc(void* fd) {
	CalendarCommand command;
	CalendarResponse response;
	CalendarEntry* temp_entry;
	Calendar* get_returns;
	int command_status;
	int client_fd = *((int*)fd);
	
	recv(client_fd, (char*)&command, sizeof(CalendarCommand), 0);
	if(command.command_code == ADD_EVENT) {                             
  	temp_entry = (CalendarEntry*)malloc(sizeof(CalendarEntry));       
    *temp_entry = command.event;
		pthread_mutex_lock(&calendar_mutex);
    command_status = CalendarAdd(temp_entry, command.username);
		pthread_mutex_unlock(&calendar_mutex);
    if(command_status != 0) {                                         
    	response.response_code = command_status;                        
    } else {                                                          
    	response.response_code = ADD_SUCCESS;                           
    }                                                                 
    send(client_fd, (char*)&response, sizeof(CalendarResponse),0);         
  } else if (command.command_code == REMOVE_EVENT) {
		pthread_mutex_lock(&calendar_mutex);                  
    command_status = CalendarRemove(&command.event, command.username);
		pthread_mutex_unlock(&calendar_mutex);
  if(command_status != 0) {                                         
  	response.response_code = command_status;                        
  	} else {                                                          
  		response.response_code = REMOVE_SUCCESS;                        
  	}                                                                 
  	send(client_fd, (char*)&response, sizeof(CalendarResponse), 0);        
  } else if (command.command_code == UPDATE_EVENT) {                  
  	CalendarEntry locate_entry;                                       
    locate_entry.date = command.event.date;                           
    locate_entry.start_time = command.event.start_time;               
    locate_entry.end_time.empty = 1;                                  
    pthread_mutex_lock(&calendar_mutex);
		command_status =                                                  
    	CalendarUpdate(&locate_entry, &command.event, command.username);
    pthread_mutex_unlock(&calendar_mutex);                   
   	if(command_status != 0) {                                         
    	response.response_code = command_status;                        
    } else {                                                          
    	response.response_code = UPDATE_SUCCESS;                        
    }                                                                 
    send(client_fd, (char*)&response, sizeof(CalendarResponse), 0); 	
	} else if (command.command_code == GET_EVENTS) {
  	CalendarEntry* current_entry = 0;     
		pthread_mutex_lock(&calendar_mutex);                            
    get_returns = CalendarGetEntries(&command.event, command.username);
		pthread_mutex_unlock(&calendar_mutex);
    current_entry = ListFirst(get_returns->entries);                  
    while(current_entry) {                                            
    	response.response_code = GET;                                   
      response.entry = *current_entry;                                
      send(client_fd, (char*)&response, sizeof(CalendarResponse), 0);
      current_entry = ListNext(get_returns->entries);                 
    }                                                                 
    response.response_code = GET_END;                                 
    send(client_fd, (char*)&response, sizeof(CalendarResponse), 0);
		ListFree(get_returns->entries, NULL);
		free(get_returns);
  }
	pthread_exit(0);
}

int main(int argc, char** argv) {

	char* parse_test;
	int port;
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;
	int listen_fd = 0;
	int incoming_fd;
	struct sockaddr_storage client_address;
	socklen_t address_length;
	int yes=1;

	if(argc == 1) {
		fprintf(stderr, "No listen port provided\n");
		exit(1);
	}
	
	port = strtol(argv[1], &parse_test, 0);
	if(port < 30001 || port > 40000 || *parse_test != '\0') {
		fprintf(stderr, "Port is not valid (30001 - 40000)\n");
		exit(1);
	}	
	
  if(CalendarInit() != 0) {                                                     
    fprintf(stderr, "Failed to initialize Calendar subsystem\n");               
    exit(1);                                                                    
  }

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;    
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;  

	if ((status = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
	}

	listen_fd = 
		socket(servinfo->ai_family, 
					 servinfo->ai_socktype, 
					 servinfo->ai_protocol);	

	if(listen_fd == -1) {
		fprintf(stderr, "Failed to obtain socket\n");
		exit(1);
	}
	
  setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));	
	if(bind(listen_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {

		fprintf(stderr, "Failed to bind socket to port\n");
		exit(1);
	}

	freeaddrinfo(servinfo);

	if(listen(listen_fd, 10) == -1) {
		fprintf(stderr, "Failed to listen on port\n");
		exit(1);
	}

	while(1) {

		address_length = sizeof client_address;
		incoming_fd = accept(listen_fd, 
													(struct sockaddr *)&client_address,
													&address_length);

		if(incoming_fd == -1) {
			fprintf(stderr, "Failed to accept incoming connection/n");
		}	else {
			pthread_t attributes;
			pthread_create(&attributes, NULL, &child_proc, (void*)&incoming_fd);
		}
	}

	return 0;
}
