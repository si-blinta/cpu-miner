#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include <stdio.h>
#include "../include/blockHeader.h"
#ifndef DPU_BINARY
#define DPU_BINARY "bin/dpu-miner"
#endif

void HOST_TOOLS_send_id(struct dpu_set_t set){
  struct dpu_set_t dpu;
  uint32_t id = 0;
  DPU_FOREACH(set,dpu,id){
      DPU_ASSERT(dpu_copy_to(dpu, "dpu_id", 0,&id,sizeof(uint32_t)));
  }
}

int main(void) {
  srand(time(NULL));
  uint32_t golden_nonce = UINT32_MAX; // initialize it as unvalid nonce
  blockHeader bh;
  uint8_t target[SIZE_OF_SHA_256_HASH];
  uint32_t nb_dpus = 0;
  /**
   * Allocating DPUS
  */
  struct dpu_set_t set, dpu;
  DPU_ASSERT(dpu_alloc(10, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
  DPU_ASSERT(dpu_get_nr_dpus(set,&nb_dpus));
  printf("ALLOCATED : %d DPUs \n",nb_dpus);

  //Generating block header and calculating target hash
  generate_block_header(&bh);                 
  calculate_target_from_bits(bh.bits,target); 
  print_256_bits_integer(target,"Target Hash");

  /**
   * Broadcasting the blockHeader , the target hash and number of dpus allocated to all DPUs
  */

  DPU_ASSERT(dpu_broadcast_to(set, "dpu_block_header", 0,&bh,sizeof(bh), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_target", 0,&target,sizeof(target), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_nb", 0,&nb_dpus,sizeof(nb_dpus), DPU_XFER_DEFAULT));
  /**
   * Sending IDs to each DPU.
  */
  HOST_TOOLS_send_id(set);
  /**
   * Launching in Synchronous way.
  */
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
 
  DPU_FOREACH(set, dpu) {
      DPU_ASSERT(dpu_log_read(dpu,stdout));
      DPU_ASSERT(dpu_copy_from(dpu,"dpu_nonce",0,&golden_nonce,sizeof(uint32_t)));
      if(golden_nonce != UINT32_MAX ){
        bh.nonce = golden_nonce;
        printf("--------------------------MINED A BLOCK--------------------------\n");
        /*print_block_header(bh);
        printf("little endian = %08x\n",to_little_endian_32(golden_nonce));
        printf("big endian = %08x\n",golden_nonce);
        break;*/
      }
      
  }
  DPU_ASSERT(dpu_free(set));

  if(golden_nonce == UINT32_MAX ){
    printf("--------------------------FAILED--------------------------\n"); 
  }
}

/**
 * Final Program : 
 *      -step 1:
 *          -Generate a blockHeader.
 *          -Allocate as much DPUs as possible.
 *          -Send IDs to DPUs,blockHeader and target hash.
 *          -Launch DPU program.
 *      -step 2:
 *          -Wait till a DPU with a golden nonce finishes. ( Maybe wait for some message / signal , use a thread for waiting ?)
 *          -Interrupt all DPUs.
 *          -Reset all ressources of DPUS. ( some resources, but not all of them, are reset before booting)
 *          -Back to step 1.
*/

/**
 * Performance Test Program : 
 *      -step 1:
 *          -Let user choose how many DPUs he wants to test with.
 *          -Let user choose how many tasklets he wants to test with.
 *          -Compile the DPU program with -DNR_TASKLETS=user_input.
 *          -Try multiple times dpu_alloc() till getting the requested number.
 *      -step 2:
 *          -Create a new variable (double) called total_hash_rate.
 *          -Generate a blockHeader.
 *          -Allocate as much DPUs as possible.
 *          -Send IDs to DPUs,blockHeader and target hash.
 *          -Launch DPU program.
 *      -step 3:
 *         -Wait for all DPUs to finish.
 *         -Retrieve each dpu_hash_rate in a temporary variable.
 *         -Compute the sum of all dpu_hash_rate in total_hash_rate.
 *         -Print results.
*/

