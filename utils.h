#ifndef _UTILS_H
#define _UTILS_H

#ifndef __WIN32
#include <dirent.h>
#endif




char*
read_file(char* file_name, char** buffer)
{
    FILE* file;
    int file_size;

    file = fopen(file_name, "rb");
    if(file == NULL) {
	perror("response file not exist");
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
    printf(*buffer);

    fclose(file);
    
}

/* char* */
/* form_http(int status, header_t* headers, char* content) { */
/*     sprintf(http_version,  "HTTP/1.1 %d OK\r\n", status); */
/*     char* http_version[1024]; */
/*     char* response; */
/*     int response_size = strlen(http_ */
/*     int headers_lenght = sizeof(headers)/sizeof(headers[0]); */
/*     for(int i = 0; i <headers_lenght; i++) { */
/* 	header_t  h = headers[0]; */
/*     } */
    

/* } */



//TODO: the function needs a windows implementation if i shipped this to windows;
char**
list_dir()
{

}
#endif
