#ifndef HOST_TOOLS_H
#define HOST_TOOLS_H
#include "blockHeader.h"
/**
 * @brief allocates the number of dpu needed.
 * 
 * 
*/
void HOST_TOOLS_allocate_dpus(dpu_set_t* set,uint16_t nb_dpus);


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
 * @brief Broadcasts mining informations to a dpu set. (maybe we will use a structure)
 * 
 * 
*/
void HOST_TOOLS_broadcast_mining_info(dpu_set_t set,blockHeader bh, uint8_t target[SIZE_OF_SHA_256_HASH]);

/**
 * @brief Retrieve the golden nonce from a set of dpus in a efficient way.
 * 
 * 
 * 
 * @return golden nonce.
*/
uint32_t HOST_TOOLS_get_golden_nonce(dpu_set set);






#endif //HOST_TOOLS_H