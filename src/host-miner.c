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
  uint32_t nb_dpus;
  uint8_t nb_tasklets;
  HOST_TOOLS_parse_args(argc,argv,&nb_dpus,&nb_tasklets);
  struct dpu_set_t set, dpu;
  HOST_TOOLS_allocate_dpus(&set,&nb_dpus);
  HOST_TOOLS_compile(nb_tasklets);
  DPU_ASSERT(dpu_load(set,DPU_BINARY,NULL));
  
  uint32_t golden_nonce = UINT32_MAX; // initialize it as unvalid nonce
  uint32_t nb_boot      = 1000;
  blockHeader bh;
  uint32_t found = 0;
  uint8_t target[SIZE_OF_SHA_256_HASH];
  //Generating block header and calculating target hash
  generate_block_header(&bh);                 
  calculate_target_from_bits(bh.bits,target); 
  print_256_bits_integer(target,"Target Hash");
  golden_nonce = HOST_TOOLS_mine_stop_repeat(set,bh,target,nb_dpus,nb_boot,&found);
  if(found){
    bh.nonce = golden_nonce;
    print_256_bits_integer(target,"Target Hash");
    printf("--------------------------MINED A BLOCK--------------------------\n");
    print_block_header(bh);
    printf("little endian = %08x\n",to_little_endian_32(golden_nonce));
    printf("big endian = %08x\n",golden_nonce);
  }
  else{
      printf("failed\n");
    }
    
  }
