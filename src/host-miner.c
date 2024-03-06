#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include <stdio.h>
#include "../include/blockHeader.h"
#ifndef DPU_BINARY
#define DPU_BINARY "bin/dpu-miner"
#endif


int main(void) {
  srand(time(NULL));
  uint32_t golden_nonce;
  uint32_t nonce_start = 0;
  uint32_t end    = UINT32_MAX;
  blockHeader bh;
  uint8_t target[SIZE_OF_SHA_256_HASH];
  generate_block_header(&bh);                 // generate blockheader in mc.bh
  calculate_target_from_bits(bh.bits,target); // target
  print_256_bits_integer(target,"Target Hash");
  struct dpu_set_t set, dpu;
  DPU_ASSERT(dpu_alloc(1, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
  DPU_ASSERT(dpu_broadcast_to(set, "bh", 0,&bh,sizeof(bh), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "target", 0,&target,sizeof(target), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  DPU_FOREACH(set, dpu) {
    DPU_ASSERT(dpu_copy_from(dpu,"nonce",0,&golden_nonce,sizeof(uint32_t)));
    DPU_ASSERT(dpu_log_read(dpu,stdout));
  }
  DPU_ASSERT(dpu_free(set));
  if(golden_nonce != -1 ){
    bh.nonce = golden_nonce;
    printf("Success\n");
    print_block_header(bh);
    printf("little endian = %08x\n",to_little_endian_32(golden_nonce));
    printf("big endian = %08x\n",golden_nonce);
  }
  else 
    printf("FAILED\n");
}