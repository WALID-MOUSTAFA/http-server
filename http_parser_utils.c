//TODO: review the header parsing
#include "http_parser_utils.h"
#include "server.h"

static int last_was_value= 0;
static int num_of_headers = 0;
request_t request;
#define CURRENT_LINE (&request.headers[num_of_headers])

void 
compose_http_response() {

}

request_t
parse_http_request(sockfd_t sock) {
	http_parser_settings settings;
	settings.on_header_field= header_field_cb;
	settings.on_header_value= header_value_cb;
	settings.on_message_begin= message_begin_cb;
	settings.on_message_complete= message_complete_cb;	
	settings.on_url= my_url_cb;
	settings.on_headers_complete= headers_complete_cb;
	//TODO: temp place;
	http_parser *parser = malloc(sizeof(http_parser));
	http_parser_init(parser, HTTP_REQUEST);
	size_t len = 800*1024, nparsed;
	char buf[len];
	ssize_t recved;
	recved = recv(sock, buf, len, 0);
	if (recved < 0) {
		error("error in recv");
	}
	nparsed = http_parser_execute(parser, &settings, buf, recved);
	/* close(sock); */
	free(parser);
	return request;
}

int
header_field_cb (http_parser *parser, const char *p, size_t len)
{
	//field=name
	if(last_was_value) {
		num_of_headers++; // new header starts;
		CURRENT_LINE->value= NULL;
		CURRENT_LINE->value_len= 0;
		CURRENT_LINE->name= malloc(len+1);
		CURRENT_LINE->name_len= len;
		strncpy(CURRENT_LINE->name, p, len);
	}else { //continue parsing name!
		CURRENT_LINE->name_len += len;
		CURRENT_LINE->name= realloc(CURRENT_LINE->name,
					    CURRENT_LINE->name_len+1);
		strncat(CURRENT_LINE->name, p, len);
	} 

	CURRENT_LINE->name[CURRENT_LINE->name_len]= '\0';
	last_was_value=0;
	return 0; 
}


 int
header_value_cb (http_parser *parser, const char *p, size_t len)
{
	if(!last_was_value){
		CURRENT_LINE->value_len= len;
		CURRENT_LINE->value= malloc(len +1);
		strncpy(CURRENT_LINE->value, p, len);
	}else {
		CURRENT_LINE->value_len += len;
		CURRENT_LINE->value= realloc(CURRENT_LINE->value,
					     CURRENT_LINE->value_len + 1);
		strncat(CURRENT_LINE->value, p, len);
	}

	CURRENT_LINE->value[CURRENT_LINE->value_len]= '\0';
	last_was_value= 1;
	return 0;
}

int
message_begin_cb (http_parser *parser)
{
	const char* method= http_method_str(parser->method);
	request.method= method;
	return 0;
}

int
message_complete_cb (http_parser *parser)
{
	return 0;
}


 int
headers_complete_cb (http_parser *parser)
{
    //TODO: log
	return 0;
}

 int
my_url_cb (http_parser *parser, const char* p, size_t len)
 {
	 //TODO: clean up;
	 char* url= malloc(len+1);
	 memset(url, 0, len+1);
	 strncpy(url, p, len);
	 request.url= url;
	 return 0;
 }

 int
body_cb (http_parser *parser, const char* p, size_t len)
{ 
    //TODO: write code for body;
    return 0;
}

