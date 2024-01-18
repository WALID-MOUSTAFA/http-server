#ifndef _UTILS_H
#define _UTILS_H

#ifndef __WIN32
#include <dirent.h>
#endif


char*
read_file(char* file_name, char** buffer)
{
    FILE* file;
    int file_size;//

    file = fopen(file_name, "rb");
    if(file == NULL) {
	perror("response file not exist");
	*buffer = (char*)malloc(sizeof(char));
	*buffer = NULL;
	return NULL;
    }
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = (char*)malloc((file_size+1)*sizeof(char));
    size_t bytes_read = fread(*buffer, sizeof(char), file_size, file);
    if(bytes_read != file_size) {
	perror("error in reading file");
	free (*buffer);
	fclose(file);
	return NULL;
    }

    fclose(file);
    
}

char*
constant_http(my_vec_t* headers, char* content, char* status) {
    char* response;
    char http_version[1024];
    size_t response_size = 0;
    size_t headers_size = 0;
    sprintf(http_version,  "HTTP/1.1 %s \r\n", status);
    headers_size  = headers->total*sizeof(header_t);
    response_size += strlen(http_version);
    response_size += headers_size;
    response_size += strlen(content);
    response = (char*)malloc((response_size+100)*sizeof(char));
    memset(response, 0, response_size);
    strcpy(response, http_version);
    for(int i = 0; i<headers->total; ++i) {
	strcat(response, ((header_t*)my_vec_get(headers, i))->name);
	strcat(response, " : ");
	strcat(response, ((header_t*)my_vec_get(headers, i))->value);
	strcat(response, "\r\n");
    }
    strcat(response, "\r\n");
    strcat(response, content);
    return response;
}



//TODO: the function needs a windows implementation if i shipped this to windows;
char**
list_dir()
{

}
#endif
