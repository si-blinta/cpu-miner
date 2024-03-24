#include "../include/blockHeader.h"
#include <mram.h>
#include <defs.h>
#include <barrier.h>
#include "mutex.h"
MUTEX_INIT(my_mutex);

/**
 * Wram host varaibles
 * */ 
__host uint32_t  dpu_nonce;
__host uint32_t  dpu_id;    // HOST -> WRAM 4 bytes aligned : todo improve ( maybe send other information with same variable ...)
__host uint32_t  dpu_nb;
__host uint8_t dpu_target[SIZE_OF_SHA_256_HASH];
__host blockHeader dpu_block_header;
__host uint32_t  dpu_nb_boot;
__host uint32_t  dpu_found = 0;
uint32_t dpu_start = 0;         // persistent variable 
/**
 * Improvements : 
 *              - Stop all tasklets when a golden nonce is found. ( Notify the host or someting ).
 *              - Use a single 4 bytes variable to store dpu_id and dpu_nb.
 *              - Use as much stack memory as needed not more.
 * 
*/
int main(void) {
    uint8_t  tasklet_id     = me(); 
    uint32_t dpu_range     = (UINT32_MAX / dpu_nb_boot) /dpu_nb ;
    uint32_t tasklet_range = dpu_range / NR_TASKLETS;
    
    if(dpu_start == 0)// If first boot calculate dpu_start.
        dpu_start     = dpu_id * dpu_range*dpu_nb_boot;
    
    uint32_t tasklet_start = dpu_start + tasklet_id*tasklet_range ;
    uint32_t tasklet_end   = dpu_start + (tasklet_id+1) *  tasklet_range ;
    uint32_t tasklet_nonce; 
    uint32_t tasklet_found = 0;
    
    tasklet_nonce = scan_hash(dpu_block_header,dpu_target,tasklet_start,tasklet_end-1,&tasklet_found);
    
    if(tasklet_found){ // update dpu_nonce only if we have a valid tasklet_nonce.
        mutex_lock(my_mutex);
        dpu_nonce = tasklet_nonce;
        dpu_found = 1;
        mutex_unlock(my_mutex);
    }

    if(tasklet_id == 0){ // A single tasklets update dpu_start for next boot
        dpu_start =  dpu_start+dpu_range;
    }  

    return 0;
}