#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include <stdio.h>
#include "sha-256.h"
#ifndef DPU_BINARY
#define DPU_BINARY "./dpu-miner"
#endif

int main(void) {
  srand(time(NULL));
  blockHeader bh;
  generate_block_header(&bh);
  print_block_header(bh);
  uint8_t hash[SIZE_OF_SHA_256_HASH];
  struct dpu_set_t set, dpu;

  DPU_ASSERT(dpu_alloc(1, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
  DPU_ASSERT(dpu_broadcast_to(set, "block_header", 0,&bh,sizeof(bh), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

  DPU_FOREACH(set, dpu) {
    DPU_ASSERT(dpu_log_read(dpu, stdout));
  }
  DPU_ASSERT(dpu_copy_from(dpu, "hash", 0, hash, SIZE_OF_SHA_256_HASH));
  DPU_ASSERT(dpu_free(set));
  printf("I RECEIVED : \n");
  for(int i = 31; i>= 0 ; i--)printf("%02x",hash[i]);
  printf("\n");

  return 0;
}