#ifndef _SOCKET_H
#define _SOCKET_H


typedef int32_t sockfd_t;

typedef struct {
	int port;
	int addr;
	int family;
} sockinfo_t;


#ifdef __cplusplus
extren "C"
{
#endif
	sockfd_t create_listen_socket(sockinfo_t* info);
	
#ifdef __cplusplus	
}
#endif


#endif
