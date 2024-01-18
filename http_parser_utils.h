#ifndef _HTTP_PARSER_UTILS_H
#define _HTTP_PARSER_UTILS_H

#include <gtk/gtk.h>
#include "node_http_parser.h"
#include "socket.h"

#define MAX_HEADER_NUM 2000

typedef struct  {
	char* name;
	size_t name_len;
	char *value;
	size_t value_len;
} header_t; 


struct _request {
    char* from;
    char* url;
    const char* method;
    header_t headers[MAX_HEADER_NUM];
    char* body;    
};

typedef struct _request request_t;

extern void gui_log(request_t *req, char* message); //forword declaration not to include the whole header for a function, remeber ramadan problem;

request_t parse_http_request(sockfd_t sock);
void compose_http_response();

int header_field_cb (http_parser *parser, const char *p, size_t len);

int
header_value_cb (http_parser *parser, const char *p, size_t len);

int
message_begin_cb (http_parser *parser);

int
message_complete_cb (http_parser *parser);

int
headers_complete_cb (http_parser *parser);

int
my_url_cb (http_parser *parser, const char* p, size_t len);

int
body_cb (http_parser *parser, const char* p, size_t len);


extern request_t request;


#endif 
