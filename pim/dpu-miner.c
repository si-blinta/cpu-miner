#include "sha-256-dpu.h"
#include <stdio.h>
int main(void) {
    const char input[]="hello world";
    uint8_t hash[32];
    calc_sha_256(hash,input,strlen(input));
    printf("Big endian :Hash\n");
    for(int i = SIZE_OF_SHA_256_HASH-1; i >=0; i--) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}



