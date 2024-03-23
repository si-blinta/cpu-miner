#ifndef HOST_TOOLS_H
#define HOST_TOOLS_H
#include <dpu.h>
#include <assert.h>
#include "blockHeader.h"
#ifndef DPU_BINARY
#define DPU_BINARY "bin/dpu-miner"
#endif
enum test_type{
    DPU_SOL_TEST,
    TASKLET_SOL_TEST,
    DPU_HASHRATE_TEST,
    TASKLET_HASHRATE_TEST
};

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

/**
 * @brief A performance test function that will help us testing and generating csv files for both 
 *        dpu count effect on hashrate and tasklets count effects on hashrate.
 * @param nb_dpus_p The number of dpus.
 * @param nb_tasklets_p The number of tasklets.
 * @param nb_hashes_p The number of hashes to be tested on. 
 * @param type The type of test.
 */
static void HOST_TOOLS_test(uint32_t nb_dpus_p, uint32_t nb_tasklets_p, uint32_t nb_hashes_p, uint8_t type, char* path);

/**
 * @brief A performance test on tasklets, it runs scanhash multiple times it, varys number of tasklet from 1 to 24, using 10 dpus.
 *        Calculates Sol/s.
 * @param path A path for the csv file.
*/
void HOST_TOOLS_tasklet_test(char* path,uint8_t type);

/**
 * @brief A performance test on tasklets, it runs scanhash it runs scanhash multiple times,it varys number of dpus from 10 to 1000, using 10 tasklets.
 *        Calculates Sol/s.
 * @param path A path for the csv file.
*/
void HOST_TOOLS_dpu_test(char* path, uint8_t type);

















#endif // HOST_TOOLS_H