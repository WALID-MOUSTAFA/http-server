//TODO(walid): handle response
//TODO: figure out when to respond with html and when to respond with sending file.

#include <sys/socket.h>
#include "server.h"
#include "socket.h"
#include "node_http_parser.h"
#include "http_parser_utils.h"
#include "utils.h"
#include "http.h"

int server_running = 0;
sockfd_t listen_sock;
gpointer global_app_communcation;
pthread_t server_thread;
char* home_dir = "./html_dir";

void
error(char* err)
{
    perror(err);
    exit(1);
}


void*
http_response(int client_socket, request_t request)
{
    char* url;
    char* file_path;
    char* extention;
    get_url(request, &url);
    file_path = file_path_from_url(home_dir, url);
    extention = strrchr(file_path, '.');
    if(extention == NULL) {
	extention = "";
    }
    
    if(is_dir(file_path)) { 
	handle_dir(client_socket, file_path); 
    } else if(strstr(extention, "html") == NULL
	      &&strstr(extention, "css") == NULL) {
	send_downloadable_file(client_socket, file_path);
    }  else {
	send_file_content(client_socket, file_path);
    }
    
    free(url);
}




void
handle_new_connections(int client_socket)
{
    request_t parsed_http_request;
    //parse http request using http parser module;
    parsed_http_request = parse_http_request(client_socket);
    http_response(client_socket, parsed_http_request);
    //cleaning
    free(request.url);
    //TODO: free headers
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

    pthread_create(&server_thread, NULL, walid_create_socket,
		   (void*)&handle_new_connections);
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
