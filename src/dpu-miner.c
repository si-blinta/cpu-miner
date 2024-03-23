#include "../include/blockHeader.h"
#include <mram.h>
#include <defs.h>
#include <barrier.h>
#include "mutex.h"
enum test_type{
    DPU_SOL_TEST,
    TASKLET_SOL_TEST,
    DPU_HASHRATE_TEST,
    TASKLET_HASHRATE_TEST
};
/**
 * Wram host varaibles
 * */ 
__host uint32_t    dpu_id;    
__host uint32_t    dpu_nb;
__host uint8_t     dpu_target[SIZE_OF_SHA_256_HASH];
__host blockHeader dpu_block_header;
__host uint32_t    dpu_nb_hashes;
__host uint32_t    dpu_test_type;
int main(void) {
    uint8_t  tasklet_id     = me();
    uint32_t dpu_range     = dpu_nb_hashes / dpu_nb ;
    uint32_t tasklet_range = dpu_range / NR_TASKLETS;
    uint32_t dpu_start     = dpu_id * dpu_range;
    uint32_t tasklet_start = dpu_start + tasklet_id*tasklet_range ;
    uint32_t tasklet_end   = dpu_start + (tasklet_id+1) *  tasklet_range ;
    uint32_t tasklet_hashes_done = 0;
    if(dpu_test_type == DPU_HASHRATE_TEST || dpu_test_type == TASKLET_HASHRATE_TEST)
        
        hash_rate_test(tasklet_range);
    
    else

        tasklet_hashes_done = scan_hash_test(dpu_block_header,dpu_target,tasklet_start,tasklet_end-1);
    
    return 0 ;
}
