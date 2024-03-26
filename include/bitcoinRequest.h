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


/**
 * @brief This function sends a request to the bitcoin server to get a block header.
 * 
*/
int get_block();

/**
 * @brief This function sends a block header.
 * 
*/
int send_block();

/**
 *@brief This function allows the server to check if the block is valid. 
*/

int verify_block();




















#endif