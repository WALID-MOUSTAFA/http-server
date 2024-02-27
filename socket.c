#include <fcntl.h>
#include "server.h"
#include "socket.h"

int port = 5000;

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
	error_check(listener_socket, -1,
		    "Error, in creating socket descriptor");
	//temperoray:
	setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR,
		   &(int){1}, sizeof(int));
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

 void*
walid_create_socket(void* handler)
{
    //TODO(walid): delegate all the socket creation to 
    //create_listen_socket function	
    struct sockaddr_in client_sock_addr;
    sockfd_t listen_socket;	
    sockinfo_t listen_socket_info = {
	.port =	 port,
	.addr =	 INADDR_ANY,
    };
    listen_socket = create_listen_socket(&listen_socket_info);
    struct epoll_event events[2]; 
    struct epoll_event event;
    int epoll_fd;

    //init epoll API
    epoll_fd = epoll_create(1);
    event.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;
    event.data.fd = listen_socket;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_socket, &event);	

    //main server event loop
    while(1) {
	int event_ready = epoll_wait(epoll_fd, events, 50, 1000);
	if(event_ready < 0) exit(1); //TODO: log error
	for(int i= 0; i < event_ready; i++){
	    int client_sockfd;
	    if(events[i].data.fd == listen_socket) {
		socklen_t client_sock_addr_len=
		    sizeof(client_sock_addr);
		client_sockfd= accept(listen_socket,
				      (struct sockaddr *) &client_sock_addr,
				      &client_sock_addr_len);
		/* Note: operations on sockets handled by
		   epoll is non-blocking by default
		   even without setting O_NONBLOCK flag.
		*/
		fcntl(client_sockfd, F_SETFL, O_NONBLOCK);
				
		
		event.data.fd = client_sockfd;
		event.events= EPOLLIN;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &event);
				
	    }else if (events[i].events & EPOLLIN) {
		client_sockfd= events[i].data.fd;
		printf("client %d connected.\n", client_sockfd);
		//new_connection_handle(client_sockfd);
		( (void(*)(int))  handler)(client_sockfd);
	    } 

	}
    }//endwhile
	
	
    return NULL;
}
