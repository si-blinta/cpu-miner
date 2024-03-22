#ifndef HOST_TOOLS_H
#define HOST_TOOLS_H
#include <dpu.h>
#include <assert.h>
#include "blockHeader.h"
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
 * @brief Retrieve the golden nonce from a set of dpus in a efficient way.
 * 
 * 
 * 
 * @return golden nonce.
*/
uint32_t HOST_TOOLS_get_golden_nonce(struct dpu_set_t set);







uint32_t HOST_TOOLS_mine_stop_repeat( struct dpu_set_t set,blockHeader bh,uint8_t target[SIZE_OF_SHA_256_HASH],uint32_t nb_dpus,uint32_t nb_boot);









#endif // HOST_TOOLS_H