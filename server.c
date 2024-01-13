//TODO(walid): handle response
//TODO: figure out when to respond with html and when to respond with sending file.
#include <fcntl.h>
#include <sys/socket.h>
#include "server.h"
#include "socket.h"
#include "vec_queue.h"
#include "node_http_parser.h"
#include "http_parser_utils.h"
#include "utils.h"


int server_running = 0;
sockfd_t listen_sock;
gpointer global_app_communcation;
my_vec_t clients;
pthread_t server_thread;
//TODO: replace those with config file
int port= 5000;
char* home_dir = "./html_dir";

void
error(char* err)
{
    perror(err);
    exit(1);
}

void
init_client_vec()
{
    my_vec_init(&clients);
}

static void
clients_iterate()
{
    
}



static void*
walid_create_socket()
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
				
		printf("client %d connected.\n", client_sockfd);
		event.data.fd = client_sockfd;
		event.events= EPOLLIN;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sockfd, &event);
				
	    }else if (events[i].events & EPOLLIN) {
		client_sockfd= events[i].data.fd;
		handle_new_connections(client_sockfd);
	    } 

	}
    }//endwhile
	
	
    return NULL;
}


void
get_url(char** url)
{
    size_t url_length = strlen(request.url);
    *url = (char*)malloc(url_length+1*sizeof(char));
    /* memset(url, 0, sizeof(url)); */
    strncpy(*url, request.url, strlen(request.url));
    (*url)[url_length] = '\0';  // Null-terminate the string

}

void*
send_file_content(int socket)
{
    char* response;
    char* response_headers;
    size_t response_size;
    char* url;
    char* file_path;
    char* file_content;

    my_vec_t headers;
    my_vec_init(&headers);
    header_t h1 = {
	.name="Content-Type",
	.value="text/html"
    };
    my_vec_add(&headers, &h1);
    
    return NULL;
    
    
    get_url(&url);
    file_path = (char*)malloc((strlen(home_dir)+strlen(url))*sizeof(char)+1);
    memset(file_path, 0, sizeof(file_path));
    if(file_path == NULL) {
	printf("error in line: %d\n", __LINE__);
	exit(0);
    }
    strcat(file_path, home_dir);
    strcat(file_path, url);

    read_file(file_path, &file_content);
    
    response_headers = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    response_size = (strlen(response_headers)+strlen(file_content))*sizeof(char);
    response = (char*) malloc(response_size + 1);
    if(response == NULL) {
	printf("error in line: %d\n", __LINE__);
	exit(0);
    }
    memset(response, 0, sizeof(response));
    strcat(response, response_headers);
    if(file_content == NULL) {
	//handle null
	printf("file not found");
	exit(0);
    } else {
	strcat(response, file_content);
    }
    send(socket, response, strlen(response), 0);
    
    free(url);
    free(file_path);
    free(response);
    close(socket);

    return NULL;
}

void*
http_response(int client_socket, request_t request)
{
    send_file_content(client_socket);
    /* strcat(response, content); */
    /* send(client_socket, response, strlen(response), 0); */
    /* close(client_socket); */
    /* free(content); */
    /* memset(response, 0, sizeof(response)); */
    /* return NULL; */
}


//TODO: pass the request down instead of using it globally across compilation units;
void*
handle_new_connections(int client_socket)
{
    request_t parsed_http_request = parse_http_request(client_socket);
    http_response(client_socket, parsed_http_request);
    return NULL;
}


 void
activate_server()
{
    char* port_s;
    int port;
    server_running= 1;
    /* char* port_s= argv[1] != NULL ? argv[1] : "5000"; */
    port_s=  "5000";
    port= atoi(port_s);
    printf("server is up and listening on port: %s\n",
	   port_s);	

    pthread_create(&server_thread, NULL, walid_create_socket, NULL);
}

 void
stop_server()
{
    server_running=0;
    close(listen_sock);
    pthread_cancel(server_thread);
    exit(0);
}


void
connect_communication_signals()
{
    g_signal_connect(global_app_communcation,
		     "activate_server",
		     G_CALLBACK(activate_server),
		     NULL);

	
    g_signal_connect(global_app_communcation,
		     "stop_server",
		     G_CALLBACK(stop_server),
		     NULL);
}


int
main(int argc, char **argv)
{
    activate_server();
    signal(SIGTERM, stop_server);
    signal(SIGINT, stop_server);

    //cleanup
    pthread_join(server_thread, NULL);
    return 0;
	
}
