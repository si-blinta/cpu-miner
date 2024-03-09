#include "../include/blockHeader.h"
#include "../include/hostTools.h"
#ifndef DPU_BINARY
#define DPU_BINARY "bin/dpu-miner"
#endif


/**
 * Questions:
 *          - How can i interrupt Dpus.
 *          - How can i reset the ressources in DPUS. ( is it needed in our case ? ) 
 *          - How can i wait for one dpu to end.      ( multi threading ? )
*/



int main(int argc, char** argv) {
  srand(time(NULL));
  if(argc < 3){
    fprintf(stderr,"Usage : %s [nb_dpus] [nb_tasklets]\n",argv[0]);
    fprintf(stdout,"0 if you want to allocate all available DPUs\n");
    exit(EXIT_FAILURE);
  }
  if(atoi(argv[2]) <= 0 ||  atoi(argv[2]) > 24 || atoi(argv[1]) < 0 || atoi(argv[1]) > 1280){
    fprintf(stderr,"1 <= nb_tasklets <= 24  0 <= nb_dpus <= 1280 \n");
    exit(EXIT_FAILURE);
  }
  uint32_t nb_dpus     = atoi(argv[1]);
  uint8_t  nb_tasklets = atoi(argv[2]);
  if(  nb_dpus == 0 )
    nb_dpus = DPU_ALLOCATE_ALL;
  struct dpu_set_t set, dpu;
  HOST_TOOLS_allocate_dpus(&set,&nb_dpus);
  HOST_TOOLS_compile(nb_tasklets);
  DPU_ASSERT(dpu_load(set,DPU_BINARY,NULL));
  
  uint32_t golden_nonce = UINT32_MAX; // initialize it as unvalid nonce
  blockHeader bh;
  uint8_t target[SIZE_OF_SHA_256_HASH];
  //Generating block header and calculating target hash
  generate_block_header(&bh);                 
  calculate_target_from_bits(bh.bits,target); 
  print_256_bits_integer(target,"Target Hash");
  /**
   * Broadcasting the blockHeader , the target hash and number of dpus allocated to all DPUs
  */
  DPU_ASSERT(dpu_broadcast_to(set, "bh", 0,&bh,sizeof(bh), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "target", 0,&target,sizeof(target), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_nb", 0,&nb_dpus,sizeof(nb_dpus), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_nonce", 0,&golden_nonce,sizeof(golden_nonce), DPU_XFER_DEFAULT));
  /**
   * Sending IDs to each DPU.
  */
  HOST_TOOLS_send_id(set);
  /**
   * Launching in Synchronous way.
  */
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  /**
   * Copying nonces :
   * Here again i need to check if its a valid nonce and break if possible.
   * Now i just copy the result if my nonce is not valid.
   * So basically i wait for all dpus to finish which is very useless.
  */
  DPU_FOREACH(set, dpu) {
      DPU_ASSERT(dpu_copy_from(dpu,"dpu_nonce",0,&golden_nonce,sizeof(uint32_t)));
      if(golden_nonce != UINT32_MAX ){
        bh.nonce = golden_nonce;
        printf("--------------------------MINED A BLOCK--------------------------\n");
        print_block_header(bh);
        printf("little endian = %08x\n",to_little_endian_32(golden_nonce));
        printf("big endian = %08x\n",golden_nonce);
        break;
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
