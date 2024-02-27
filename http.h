#include "vec_queue.h"

void* send_file_content(int socket, char* file_path);
void* handle_dir(int socket, char* url_path);
void send_downloadable_file(int socket, char* file_name);
