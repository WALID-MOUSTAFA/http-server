#include "server.h"
#include "socket.h"


void
error_check(int statement, int expected_error, char* msg)
{
	if(!statement == expected_error) {
		error(msg); 
		exit(1);
	} 
} 

sockfd_t
create_listen_socket(sockinfo_t *info)
{

	sockfd_t listener_socket;
	struct sockaddr_in server_sock_addr;

	listener_socket= socket(AF_INET, SOCK_STREAM, 0);
	error_check(listener_socket, -1, "Error, in creating socket descriptor");
	
	server_sock_addr.sin_port= htons(info->port);
	server_sock_addr.sin_family= AF_INET;
	server_sock_addr.sin_addr.s_addr= info->addr;
	
	int binding_result= bind(listener_socket,
				 (struct sockaddr*) &server_sock_addr,
				 sizeof(server_sock_addr));
	
	error_check(binding_result, -1, "Error, faied in binding socket");
	int listening_result =  listen(listener_socket, 1);	
	error_check(listening_result, -1, "Error, faied in listening to port ");

	return listener_socket;
}
