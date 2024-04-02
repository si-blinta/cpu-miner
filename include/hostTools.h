#ifndef HOST_TOOLS_H
#define HOST_TOOLS_H
#include <dpu.h>
#include <assert.h>
#include "blockHeader.h"
#include "../include/bitcoinRequest.h"
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include "../include/config.h"
/**
 * @brief This function simply parse arguments from commandline and update variables.
 * @param argc Number of arguments.
 * @param argv The values of arguments.
 * @param nb_dpus The pointer to nb_dpu variable to update.
 * @param nb_tasklets The pointer to nb_tasklets variable to update.
 * @param nb_boots The pointer to nb_boots variable to update.
 * @param nb_blocks_to_mine The pointer to nb_blocks_to_mine to update.
*/

void HOST_TOOLS_parse_args(int argc, char** argv, uint32_t* nb_dpus,uint8_t* nb_tasklets,uint32_t* nb_boots,uint32_t* nb_blocks_to_mine);


/**
 * @brief allocates the number of dpu needed.
 * @param set A pointer to a dpu_set_t structure.
 * @param nb_dpus A pointer to the number of dpus to allocate,
 *                It stores the actual number of dpus allocated.
 * 
*/
void HOST_TOOLS_allocate_dpus(struct dpu_set_t* set,uint32_t* nb_dpus);


/**
 * @brief Sends id to each DPU allocated.
 * @param set The set to send ids to .
*/
void HOST_TOOLS_send_id(struct dpu_set_t set);

/**
 * @brief Compile DPU program with the number of tasklets needed.
 * @param nb_tasklets Number of tasklets to compile with
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
 * @param nb_boots The number of times we are booting the dpus, useful for dpus to compute their nonce space.
 * @param host_found A pointer to the variable to check if a nonce is found.
 * @return The nonce, UINT32_MAX if not valid. 
 * @note you will need to compare host_found : if its = 1 , the nonce is valid else its not
*/
uint32_t HOST_TOOLS_mine_multiple_boot( struct dpu_set_t set,blockHeader bh,uint8_t target[SIZE_OF_SHA_256_HASH],
                                    uint32_t nb_dpus,uint32_t nb_boots, uint32_t* host_found);

/**
 * @brief Connect to a server using udp.
 * @param server_ip The server's IP.
 * @param server_port The server's port.
 * @param server_addr A pointer to a server address.
 * @param sockfd      A pointer to socket file descriptor.
 * @return returns -1 on failure , 0 on sucess;
*/
int HOST_TOOLS_connect(const char* server_ip, int server_port,struct sockaddr_in* server_addr,int* sockfd);

/**
 * @brief This function simulates requesting blocks from the bitcoin server, mining and sending the mined block back.
 * @param server_addr A pointer to a server address.
 * @param sockfd      A pointer to socket file descriptor.
 * @param set The dpu_set structure , this set must be allocated before the call.
 * @param nb_dpus The number of dpus used ,useful for dpus to compute their nonce space.
 * @param nb_boots The number of times we are booting the dpus, useful for dpus to compute their nonce space.
 * @param number_of_blocks_to_mine How many blocks to mine in the simulation.
*/

void HOST_TOOLS_mine(struct sockaddr_in server_addr,int sockfd,struct dpu_set_t set,uint32_t nb_dpus,
                    size_t number_of_blocks_to_mine,uint32_t nb_boots);
#endif // HOST_TOOLS_H