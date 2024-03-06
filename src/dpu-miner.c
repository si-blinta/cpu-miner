#include "../include/blockHeader.h"
#include <mram.h>
//__mram_noinit mining_context m_cntx; 
__mram_noinit uint8_t target[SIZE_OF_SHA_256_HASH]; 
__mram_noinit blockHeader bh; 
__host uint32_t nonce;
int main(void) {
    __dma_aligned uint8_t t[SIZE_OF_SHA_256_HASH];
    mram_read(&target,&t,SIZE_OF_SHA_256_HASH);
    __dma_aligned blockHeader bhh;
    mram_read(&bh,&bhh,sizeof(bh));
    printf("received-----------------------------------------------------------------------------\n");
    print_block_header(bhh);
    nonce = scan_hash(bhh,t,0,UINT32_MAX);
    return 0;
}



