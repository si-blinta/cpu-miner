#include "../include/blockHeader.h"
#include <mram.h>
#include <defs.h>
#include <barrier.h>
#include "mutex.h"
MUTEX_INIT(my_mutex);

//Global variable to stop tasklets.
extern uint8_t finish;

/**
 * Wram host varaibles
 * */ 
__host uint32_t  dpu_nonce;
__host uint32_t  dpu_id;    // HOST -> WRAM 4 bytes aligned : todo improve ( maybe send other information with same variable ...)
__host uint32_t  dpu_nb;
__host uint8_t dpu_target[SIZE_OF_SHA_256_HASH];
__host blockHeader dpu_block_header;

/**
 * Improvements : 
 *              - Stop all tasklets when a golden nonce is found. ( Notify the host or someting ).
 *              - Use a single 4 bytes variable to store dpu_id and dpu_nb.
 *              - Use as much stack memory as needed not more.
 * 
*/
int main(void) {
    uint8_t  tasklet_id     = me();
    uint32_t dpu_range     = UINT32_MAX / dpu_nb ;
    uint32_t tasklet_range = dpu_range / NR_TASKLETS;
    uint32_t dpu_start     = dpu_id * dpu_range;
    uint32_t tasklet_start = dpu_start + tasklet_id*tasklet_range ;
    uint32_t tasklet_end   = dpu_start + (tasklet_id+1) *  tasklet_range ;
    uint32_t tasklet_nonce;  
    tasklet_nonce = scan_hash(dpu_block_header,dpu_target,tasklet_start,tasklet_end-1);
    if(tasklet_nonce != UINT32_MAX){ // update dpu_nonce only if we have a valid tasklet_nonce.
        mutex_lock(my_mutex);
        finish = 1;                  // Interrupt other tasklets.
        dpu_nonce = tasklet_nonce;
        mutex_unlock(my_mutex);
    }
    return 0;
}
/**
 * Performance Test program : 
 *      -step 1:
 *          -mram read all the variables needed.
 *          -create an array : hash_rate[NR_TASKLET].
 *          -add the global variable __host dpu_hash_rate.
 *      -step 2:
 *          -scan_hash_test().
 *          -pass &hash_rate[tasklet_id] to the function, the function will fill hash_rate[tasklet_id] with the hash_rate of the tasklet. 
 *      -step 3:
 *          - Use a barrier_wait().
 *          - Use one tasklet , for example tasklet 0, to compute the sum of all hash_rate.
 *          - Fill the variable dpu_hash_rate with the sum. ( the host will take it from here )
*/