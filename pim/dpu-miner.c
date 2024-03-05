#include "sha-256-dpu.h"
#include <stdio.h>
__mram_noinit blockHeader block_header;
__host uint8_t hash[SIZE_OF_SHA_256_HASH];
void print_block_header(const blockHeader blockHeader) {
    printf("Big endian :Version: %08x\n", blockHeader.version);
    printf("Big endian :Previous Hash: ");
    for (int i = 0; i < sizeof(blockHeader.previous_hash); i++) {
        printf("%02x", (unsigned char)blockHeader.previous_hash[i]);
    }
    printf("\nBig endian :Merkle Root Hash: ");
    for (int i = SIZE_OF_SHA_256_HASH-1; i >= 0; i--) {
        printf("%02x", (unsigned char)blockHeader.merkle_root_hash[i]);
    }
    printf("\nBig endian :Time: %08x\n", blockHeader.time);
    printf("Big endian :Bits: %08x\n", blockHeader.bits);
    printf("Big endian :Nonce: %08x\n", blockHeader.nonce);
}
int main(void) {
    const char input[]="hello world";
    calc_sha_256(hash,input,strlen(input));
    print_block_header(block_header);
}



