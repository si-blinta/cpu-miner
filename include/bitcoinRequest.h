#ifndef BITCOIN_REQUEST_H
#define BITCOIN_REQUEST_H
#define BLOCK_HEADER_PACKET_SIZE 81
#define BITCOIN_PORT 8831
#include "blockHeader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>

enum bitcoin_request{
    GET,
    PUT,
    ERROR
};

/**
 * @brief This function sends a request to the bitcoin server to get a block header.
 * 
*/
int get_block(const struct sockaddr_in* dest_addr, int sockfd);

/**
 * @brief This function sends a block header.
 * 
*/
int send_block(const struct sockaddr_in* dest_addr, int sockfd,blockHeader* block_header,socklen_t len);

/**
 *@brief This function allows the server to check if the block is valid. 
*/
int verify_block(blockHeader block_header);



void deserialize(blockHeader *data, const char *buffer);
void serialize(const  blockHeader *data, char *buffer);
















#endif