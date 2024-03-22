#include "../include/blockHeader.h"
#include "../include/hostTools.h"
#ifndef DPU_BINARY
#define DPU_BINARY "bin/dpu-miner"
#endif



/**
 * Questions:
 *          - How can i interrupt Dpus.
 *          - I found how to check if a dpu had finished : dpu_poll_dpu(dpu.dpu,&running,&fault), include low level library <dpu_runner.h>
 *           (idea : maybe use a tasklet checking for a host_variable changing if so , alert other tasklets)
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
  uint32_t nb_boot      = 2;
  blockHeader bh;
  uint8_t target[SIZE_OF_SHA_256_HASH];
  //Generating block header and calculating target hash
  generate_block_header(&bh);                 
  calculate_target_from_bits(bh.bits,target); 
  print_256_bits_integer(target,"Target Hash");
  golden_nonce = HOST_TOOLS_mine_stop_repeat(set,bh,target,nb_dpus,nb_boot);
  if(golden_nonce != UINT32_MAX){
    bh.nonce = golden_nonce;
    printf("--------------------------MINED A BLOCK--------------------------\n");
    print_block_header(bh);
    printf("little endian = %08x\n",to_little_endian_32(golden_nonce));
    printf("big endian = %08x\n",golden_nonce);
  }
  else{
    //double check because uint32_max might be a solution :)
    printf("failed\n");
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
