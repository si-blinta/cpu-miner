#ifndef HOST_TOOLS_H
#define HOST_TOOLS_H
#include <dpu.h>
#include <assert.h>
#include "blockHeader.h"
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#define DEBUG 1

/**
 * @brief This function simply parse arguments from commandline and update variables.
 * @param argc Number of arguments.
 * @param argv The values of arguments.
 * @param nb_dpus The pointer to nb_dpu variable to update.
 * @param nb_tasklets The pointer to nb_tasklets variable to update.
 * 
*/

void HOST_TOOLS_parse_args(int argc, char** argv, uint32_t* nb_dpus,uint8_t* nb_tasklets);


/**
 * @brief allocates the number of dpu needed.
 * 
 * 
*/
void HOST_TOOLS_allocate_dpus(struct dpu_set_t* set,uint32_t* nb_dpus);


/**
 * @brief Sends id to each DPU allocated.
 * 
*/
void HOST_TOOLS_send_id(struct dpu_set_t set);

/**
 * @brief Compile DPU program with the number of tasklets needed.
 * 
 * 
*/
void HOST_TOOLS_compile(uint8_t nb_tasklets);


/**
 * 
 * @brief This function does the mining process utilizing the persistence of objects in dpu, it only broadcast data one time
 *        then it launches dpus, the dpus will compute their mining space automatically and they will update their mining space
 *        every time they launch without the need for data transfer.
 * @param set The dpu_set structure , this set must be allocated before the call.
 * @param bh  The block header.
 * @param target The target hash.
 * @param nb_dpus The number of dpus used ,useful for dpus to compute their nonce space.
 * @param nb_boot The number of times we are booting the dpus, useful for dpus to compute their nonce space.
 * @return The nonce, UINT32_MAX if not valid.
 * @note   Even id the result is UINT32_MAX we need to double check because it is a possible value for a nonce.
*/
uint32_t HOST_TOOLS_mine_stop_repeat( struct dpu_set_t set,blockHeader bh,uint8_t target[SIZE_OF_SHA_256_HASH],uint32_t nb_dpus,uint32_t nb_boot, uint32_t* host_found);

/**
 * @brief Connect to the bitcoin to a server using udp.
 * 
*/
int HOST_TOOLS_connect(const char* server_ip, int server_port,struct sockaddr_in* server_addr,int* sockfd);


#endif // HOST_TOOLS_H