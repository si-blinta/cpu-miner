#ifndef BITCOIN_REQUEST_H
#define BITCOIN_REQUEST_H
#include "blockHeader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/config.h"
enum bitcoin_request{
    GET,
    PUT,
    ERROR
};

/**
 * @brief This function sends a request to the bitcoin server to get a block header.
 * @param dest_addr The destination adress.
 * @param sockfd    The socket to send from.
*/
int get_block(const struct sockaddr_in* dest_addr, int sockfd);

/**
 * @brief This function sends a block header.
 * @param dest_addr The destination adress.
 * @param sockfd    The socket to send from.
 * @param blockHeader The block header to send.
*/
int send_block(const struct sockaddr_in* dest_addr, int sockfd,blockHeader* block_header,socklen_t len);

/**
 *@brief This function allows the server to check if the block is valid. 
 *@param block_header The block header. 
*/
int verify_block(blockHeader block_header);

/**
 * @brief This function serialize a block header in a buffer.
 * @param data The adress of block header.
 * @param buffer The buffer to store it in.
*/

void serialize(const  blockHeader *data, char *buffer);

/**
 * @brief This function exctracts block header data from a buffer.
 * @param data The adress of block header.
 * @param buffer The buffer to extract from.
 * @note it is used this way deserialize(&block_header,buffer+1). buffer+1 because the first byte is a flag.
*/
void deserialize(blockHeader *data, const char *buffer);
















#endif