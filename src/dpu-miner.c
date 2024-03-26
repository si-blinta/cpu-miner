#include "../include/blockHeader.h"
#include <mram.h>
#include <defs.h>
#include <barrier.h>
#include "mutex.h"
MUTEX_INIT(my_mutex);
#if DPU_DEBUG
    BARRIER_INIT(my_barrier,NR_TASKLETS);
#endif//DPU_DEBUG
/**
 * Wram host varaibles
 * */ 
__host uint32_t  dpu_nonce;
__host uint32_t  dpu_id;    
__host uint32_t  dpu_nb;
__host uint8_t dpu_target[SIZE_OF_SHA_256_HASH];
__host blockHeader dpu_block_header;
__host uint32_t  dpu_nb_boots;
__host uint32_t  dpu_found = 0;
uint32_t dpu_start = 0;                                           // Utilize the persistence of objects in DPUs.

int main(void) {
    
    uint8_t  tasklet_id     = me();                                //Get tasklet id  
    uint32_t dpu_range     = (UINT32_MAX / dpu_nb_boots) /dpu_nb ;  //Get nonce range for a dpu
    uint32_t tasklet_range = dpu_range / NR_TASKLETS;              //Deduce nonce range for a tasklet
    
    if(dpu_start == 0)                                             // If first boot calculate dpu_start.
        dpu_start     = dpu_id * dpu_range*dpu_nb_boots;
    
    uint32_t tasklet_start = dpu_start + tasklet_id*tasklet_range ; // from where each tasklets begins
    uint32_t tasklet_end   = dpu_start + (tasklet_id+1) *  tasklet_range ;//where each tasklet ends
    uint32_t tasklet_nonce; 
    uint32_t tasklet_found = 0;                                           //flag to tell the host if mining i successful
                                                                          //or nonce range are explored and no solution is found
    
#if DPU_DEBUG
    barrier_wait(&my_barrier);
    if(tasklet_id == 0 )
    printf("[DPU %d] range =  %u =====> %u total : %u\n",dpu_id,dpu_start,dpu_start+(dpu_id+1)*dpu_range,dpu_range);    
    printf("[tasklet %d] range =  %u =====> %u total : %u\n",tasklet_id,tasklet_start,tasklet_end,tasklet_range);
#endif//DPU_DEBUG

    tasklet_nonce = scan_hash(dpu_block_header,dpu_target,tasklet_start,tasklet_end-1,&tasklet_found); // "mining"
    
    if(tasklet_found){ // update dpu_nonce only if we have a valid tasklet_nonce.
        mutex_lock(my_mutex);
        dpu_nonce = tasklet_nonce;
        dpu_found = 1;
        mutex_unlock(my_mutex);
    }

    if(tasklet_id == 0){ // A single tasklets update dpu_start for next boot : since we don't resend data about nonce range
        dpu_start =  dpu_start+dpu_range;
    }  

    return 0;
}