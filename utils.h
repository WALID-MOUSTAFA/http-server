#ifndef _UTILS_H
#define _UTILS_H

#include "http_parser_utils.h"
#include "vec_queue.h"

void removeSubString(char* minStr, const char* substr);
void* form_html_list_directory(char*** list, int nuber, char** buffer);
int is_dir(char* file_path);
void get_url(request_t request, char** url);
char* file_path_from_url(char* home_dir, char* url);
void* list_dir(char* path, char*** list, int* length);
void respond_to_dir(char* dir_name, char** buffer);
void read_file(char* file_name, char** buffer);
char* construct_headers(my_vec_t* headers, char* status);
char* construct_http(my_vec_t* headers, char* content, char* status);


#endif
