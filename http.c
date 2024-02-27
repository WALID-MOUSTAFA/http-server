#include "server.h"
#include "socket.h"
#include "http.h"
#include "http_parser_utils.h"
#include "utils.h"

void*
send_file_content(int socket, char* file_path)
{
    char* response;
    char* file_content;
    my_vec_t headers;

    //form headers vector
    my_vec_init(&headers);
    /* header_t h1 = { */
    /* 	.name="Content-Type", */
    /* 	.value="text/html" */
    /* }; */
    /* my_vec_add(&headers, &h1); */

    //send file
    read_file(file_path, &file_content);
    
    if(file_content == NULL) {
	response = construct_http(&headers, "file not found" , "404 Not Found");
    } else {
	response = construct_http(&headers, file_content , "200 OK");
    }

    send(socket, response, strlen(response), 0);

    //free resources
    free(response);
    free(file_path);
    free(file_content);
    my_vec_free(&headers);
    close(socket);
    return NULL;
}

void*
handle_dir(int socket, char* url_path) {
    char* response;
    char* content;
    my_vec_t headers;

    //build headers
    my_vec_init(&headers);
    header_t h1 = {
	.name="Content-Type",
	.value="text/html"
    };
    my_vec_add(&headers, &h1);
    //send file
    respond_to_dir(url_path, &content);
    if(strlen(content) == 0) {
	response = construct_http(&headers, "file not found" , "404 Not Found");
    } else {
	response = construct_http(&headers, content , "200 OK");
    }
    send(socket, response, strlen(response), 0);

    //free resources
    free(response);
    free(url_path);
    free(content);
    my_vec_free(&headers);
    close(socket);
    return NULL;
}

void
send_downloadable_file(int socket, char* file_name)
{
    char* extention;
    my_vec_t headers;
    header_t contentType;
    
    extention = strrchr(file_name, '.');
    if(extention == NULL) {
	extention = "";
    }
    my_vec_init(&headers);

    if(strstr(extention, "html") != NULL) {
	contentType.name = "Content-Type";
	contentType.value = "text/html";
    } else {
	contentType.name = "Content-Type";
	contentType.value = "application/octet-stream";
    }
    my_vec_add(&headers, &contentType);
    
    char contentDispositionHeaderValue[1024];
    sprintf(contentDispositionHeaderValue,
	    "attachment; filename=\"%s\"\r\n\r\n",extention+1);

    if(strstr(extention, "html") == NULL) {
	header_t contentDisposition = {
	    .name= "Content-Disposition",
	    .value = contentDispositionHeaderValue
	};
	my_vec_add(&headers, &contentDisposition);
    }


    char* headers_str = construct_headers(&headers, "200 OK"); //TODO: suppose ok;
    send(socket, headers_str, strlen(headers_str), 0);
    FILE * file = fopen(file_name, "rb");
    if(file == NULL){
	//TODO: handle error
	char* error = "file not found";
	send(socket, error, strlen(error), 0);
	close (socket);
	my_vec_free(&headers);
	return;
    }
    
    char buffer[1024];
    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
	send(socket, buffer, bytesRead, 0);
    }
    fclose(file);
    close(socket);
    my_vec_free(&headers);
}

