#include "../include/blockHeader.h"
#include <mram.h>
#include <defs.h>
#include <barrier.h>
#include "mutex.h"

BARRIER_INIT(my_barrier, NR_TASKLETS);
MUTEX_INIT(my_mutex);

/**
 * Questions : how can i stop the execution of all tasklet once one finished ?
*/

/**
 * MRAM variable : maybe i use WRAM instead since target = 32 bytes and blockheader = 80 bytes.
 * In upmem guide they suggest to put 10 -> 100 bytes variables in WRAM
*/
__mram_noinit uint8_t target[SIZE_OF_SHA_256_HASH]; 
__mram_noinit blockHeader bh;

/**
 * Wram host varaibles
 * */ 
__host uint32_t  dpu_nonce;
__host uint32_t  dpu_id;    // HOST -> WRAM 4 bytes aligned : todo improve ( maybe send other information with same variable ...)
__host uint32_t  dpu_nb;

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
    
    //start mram_read
    __dma_aligned uint8_t dpu_target_hash[SIZE_OF_SHA_256_HASH];
    mram_read(&target,&dpu_target_hash,SIZE_OF_SHA_256_HASH);
    __dma_aligned blockHeader dpu_block_header;
    mram_read(&bh,&dpu_block_header,sizeof(bh));
    //end mram_read

    printf("Tasklet %d : start = %u | end = %u \n",tasklet_id,tasklet_start,tasklet_end);
    tasklet_nonce = scan_hash(dpu_block_header,dpu_target_hash,tasklet_start,tasklet_end-1);
    
    /**
     * We will remove barrier its just slowing the program, we just use it to debug now
    */

    barrier_wait(&my_barrier);  // wait all tasklets to finish before printing results ( Debug purposes).
    printf("Tasklet %d : nonce = %08x, Stack ressources available : %u : dpu id = %d\n",tasklet_id,tasklet_nonce, check_stack(),dpu_id);
    
    if(tasklet_nonce != UINT32_MAX){ // update dpu_nonce only if we have a valid tasklet_nonce.
        mutex_lock(my_mutex);
        dpu_nonce = tasklet_nonce;
        mutex_unlock(my_mutex);
        // Interrupt other tasklets. ( maybe send something to the host to stop other dpus also ).
    }
    return 0;
}
/**
 * Final Program : 
 *      -step 1:
 *          -mram read all the variables needed.
 *      -step 2:
 *          -scan_hash()
 *      -step 3:
 *          - if tasklet_nonce is valid :
 *              - end all tasklets.
 *              - notify host ? .
*/

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