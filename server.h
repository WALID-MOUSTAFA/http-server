#ifndef _SERVER_H
#define _SERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#ifdef linux 
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <gtk/gtk.h>
#endif


#include <pthread.h>


#include "node_http_parser.h"

#ifdef __cplusplus
extren "C"
{
#endif

#define MAX_HEADER_NUM 2000
	
	extern int server_running;

	extern int listen_sock;

	extern gpointer global_app_communcation;

	extern gchar cross_logging_buffer[100];
	
	
	static void handle_new_connections(int sockfd);
 
	void error(char* err);
	
#ifdef __cplusplus	
}
#endif

#endif
