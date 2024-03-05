#include "sha-256.h"
#include <stdio.h>
int main(void) {
    srand(time(NULL));
    blockHeader bh;
    generate_block_header(&bh);
    print_block_header(bh);
    uint32_t golden_nonce = scan_hash_test(bh,UINT32_MAX);
}



