
#ifndef __WIN32
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h> //needs to be handled if ported to ms windows;
#include <sys/stat.h>
#include <string.h>
#endif

#include "utils.h"


void
removeSubstring(char* mainStr, const char* substr) {
    // Get the length of the substring
    size_t substrLen = strlen(substr);

    // Search for the substring in the main string
    char* foundPos = strstr(mainStr, substr);

    // If the substring is found
    if (foundPos != NULL) {
        // Calculate the length of the prefix (portion before the substring)
        size_t prefixLen = foundPos - mainStr;

        // Calculate the length of the suffix (portion after the substring)
        size_t suffixLen = strlen(foundPos + substrLen);

        // Move the suffix to overwrite the substring
        memmove(foundPos, foundPos + substrLen, suffixLen + 1);
    }
}

void*
form_html_list_directory(char*** list, int number, char** buffer)
{
    char* html_opening = "<html> <head> <head> </body> <ul>";
    char* html_closing = "</ul> </body> ";
    int size_of_li_tag = 27;
    int list_size = 0;
    int buffer_size = 0;
    int offset = 0;
    
    for(int i = 0; i < number; ++i) {
		list_size+=strlen((*list)[i]);
    }
    buffer_size = ((list_size*size_of_li_tag)+
				   strlen(html_opening)
				   +strlen(html_closing)
				   )*sizeof(char)*2;
    *buffer= (char*)malloc(buffer_size);
    offset+=sprintf(*buffer, "%s", html_opening);

	
	for(int i = 0; i < number; ++i) {
		char* name = strrchr((*list)[i], '/');
		if(strcmp(name, "/.") != 0) {
		offset += sprintf(*buffer+offset,
						  "<a style='display: block; margin-bottom: 4px' href='%s'> <li>%s</li> </a>",
						  (*list)[i], ++name );
		}
    }
    offset+=sprintf(*buffer+offset, "%s", html_closing);
}

int
is_dir(char* file_path) {
    struct stat sstat;
    stat(file_path, &sstat);
    return S_ISDIR(sstat.st_mode);
}


void
get_url(request_t request, char** url)
{
    size_t url_length = strlen(request.url);
    *url = (char*)malloc(url_length*sizeof(char));
    memset(*url, 0, url_length);
    strcpy(*url, request.url);
}

char*
file_path_from_url(char* home_dir, char* url)
{
    char* file_path;
    file_path = (char*)malloc((strlen(home_dir)+strlen(url))*sizeof(char)+1);
    memset(file_path, 0, sizeof(file_path));
    strcpy(file_path, home_dir);
    strcat(file_path, url);
    return file_path;
}


void*
list_dir(char* path, char*** list, int* length )
{
    DIR* dir;
    struct dirent* entry;

    // Open directory
    dir = opendir(path);
    
    // Check if directory opened successfully
    if (dir == NULL) {
        return NULL;
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
		(*length)++;
    }

    rewinddir(dir);
    *list = (char **)malloc(*length * sizeof(char *));

    for(int i = 0; i < *length; ++i) {
		//	if(strstr((*list)[i], "/..") != NULL && strstr( (*list)[i], "/.") !=NULL ){
		//	    continue;
		//	}
	
		entry = readdir(dir);
		(*list)[i] = (char*)
			malloc ((strlen(entry->d_name)+2+strlen(path)) * sizeof(char));
		if(strcmp(path, "./html_dir") != 0){
			sprintf((*list)[i], "%s/%s", path, entry->d_name);
			removeSubstring((*list)[i], "./html_dir/");
		} else {
			sprintf((*list)[i], "%s", entry->d_name);
			removeSubstring((*list)[i], "./html_dir/");
		}
    }
    
    // Close directory
    closedir(dir);
    return NULL;
}


void
respond_to_dir(char* dir_name, char** buffer)
{
    char** files_in_dir = NULL;
    int files_in_dir_length = 0;
    list_dir(dir_name, &files_in_dir, &files_in_dir_length);
    form_html_list_directory(&files_in_dir, files_in_dir_length, buffer);
    return;
}


void
read_file(char* file_name, char** buffer)
{
    FILE* file;
    int file_size;//
    
    file = fopen(file_name, "rb");
    if(file == NULL) {
		perror("response file not exist");
		*buffer = (char*)malloc(sizeof(char));
		*buffer = NULL;
		return;
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
		return ;
    }

    fclose(file);
    
}

char*
construct_headers(my_vec_t* headers, char* status)
{
    char* buffer;
    size_t buffer_size = 0;
    char http_version[1024];
    size_t headers_size = 0;
    
    sprintf(http_version, "HTTP/1.1 %s \r\n", status);
    buffer_size += sizeof(http_version);
    for(int i = 0; i < headers->total; i++) {
		headers_size += strlen(((header_t*)my_vec_get(headers, i))->name);
		headers_size += strlen(((header_t*)my_vec_get(headers, i))->value);
    }
    buffer_size += headers_size;
    buffer_size *= 2;
    buffer = (char*) malloc(buffer_size); 
    memset(buffer, 0, buffer_size);
    strcpy(buffer, http_version);
    for(int i = 0; i<headers->total; ++i) {
		strcat(buffer, ((header_t*)my_vec_get(headers, i))->name);
		strcat(buffer, " : ");
		strcat(buffer, ((header_t*)my_vec_get(headers, i))->value);
		strcat(buffer, "\r\n");
    }
    return buffer;
}

char*
construct_http(my_vec_t* headers, char* content, char* status) {
    char* response;
    char http_version[1024];
    size_t response_size = 0;
    size_t headers_size = 0;
    sprintf(http_version,  "HTTP/1.1 %s \r\n", status);
    for(int i = 0; i < headers->total; i++) {
		headers_size += strlen(((header_t*)my_vec_get(headers, i))->name);
		headers_size += strlen(((header_t*)my_vec_get(headers, i))->value);
    }
    response_size += strlen(http_version);
    response_size += headers_size;
    response_size += strlen(content);
    response = (char*)malloc((response_size+100)*sizeof(char));
    memset(response, 0, (response_size+100)*sizeof(char));
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




