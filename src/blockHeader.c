#include "../include/blockHeader.h"
uint32_t to_little_endian_32(uint32_t value) {
    return ((value & 0xFF) << 24) |
           ((value & 0xFF00) << 8) |
           ((value >> 8) & 0xFF00) |
           ((value >> 24) & 0xFF);
}
void uint32_t_to_hex(uint32_t value, char* buffer) {
    const char hex_chars[] = "0123456789abcdef";
    for (int i = 0; i < 8; i++) {
        buffer[7 - i] = hex_chars[value & 0xF];
        value >>= 4;
    }
    buffer[8] = '\0'; // Null-terminate the string
}
void uint8_t_to_hex(uint8_t value, char* buffer) {
    const char hex_chars[] = "0123456789abcdef";
    buffer[0] = hex_chars[value >> 4];
    buffer[1] = hex_chars[value & 0x0F];
}
#ifdef HOST
void generate_block_header(blockHeader *block_header) {
    // Correctly generate random values for integers
    block_header->version = (uint32_t)rand(); 			    // Random version
    block_header->time 	  = (uint32_t)time(NULL); 		    // Current time
    block_header->bits    = rand() %0x7fffff +0x008000 ;    // Mantissa maximum legal value = 0x7fffff,
															// 0x008000 is the smallest legal   source : https://wiki.bitcoinsv.io/index.php/Difficulty 
	uint8_t r             = rand() % 5 + 52;				// We limit the randomness of exponent because it may take infinite time with my potato pc 
	uint32_t exponent     = TRAILING_ZEROS(r);				// this macro will make sure to have 50 to 56 trailing zeros so the target will start with
															// at least (64-56-4) 4 zeros. 64 = nb bytes of sha256 hash, 56 = nb bytes as trailing zeros
															// 4 = minimum of bytes as leading zeros that a mantissa can have (0x008000 is the smallest legal).
	block_header->bits    = block_header->bits | exponent << 24;
	block_header->nonce   = 0 ;

    for (int i = 0; i < sizeof(block_header->previous_hash); i++) {
        block_header->previous_hash[i] = (uint8_t)(rand() % 256);
    }
    for (int i = 0; i < sizeof(block_header->merkle_root_hash); i++) {
        block_header->merkle_root_hash[i] = (uint8_t)(rand() % 256);
    }
}
#endif //HOST

void print_block_header(const blockHeader block_header) {
    printf("Big endian :Version: %08x\n", block_header.version);
	printf("Little endian :Version: %08x\n",to_little_endian_32(block_header.version));
    print_256_bits_integer(block_header.previous_hash,"Previous Hash");
    print_256_bits_integer(block_header.merkle_root_hash,"Merkle Root Hash");
    printf("\nBig endian :Time: %08x\n", block_header.time);
	printf("Little endian :Time: %08x\n",to_little_endian_32(block_header.time));
    printf("Big endian :Bits: %08x\n", block_header.bits);
	printf("Little endian :Bits: %08x\n",to_little_endian_32(block_header.bits));
    printf("Big endian :Nonce: %08x\n", block_header.nonce);
	printf("Little endian :Nonce: %08x\n",to_little_endian_32(block_header.nonce));
    char concat[CONCAT_LENGTH];
    concat_block_header(block_header,concat);
    printf("Little endian :Concatenated block header:\n%s\n",concat);
    printf("\n");
}
void print_256_bits_integer(const uint8_t value[SIZE_OF_SHA_256_HASH],const char* name){
    printf("Big endian :%s: \n",name);
    for(int i = SIZE_OF_SHA_256_HASH-1; i >= 0; i--) {
        printf("%02x", value[i]);
    }
    printf("\nLittle endian :%s: \n",name);
    for(int i = 0; i < SIZE_OF_SHA_256_HASH; i++) {
        printf("%02x", value[i]);
    }
    printf("\n");
}
void concat_block_header(const blockHeader blockHeader, char output[CONCAT_LENGTH]) {
    // Convert version to hex
    uint32_t_to_hex(to_little_endian_32(blockHeader.version), output);
    output += 8;
    
    // Convert previous_hash to hex
    for (int i = 0; i < SIZE_OF_SHA_256_HASH; i++) {
        uint8_t_to_hex(blockHeader.previous_hash[i], output);
        output += 2;
    }
    
    // Convert merkle_root_hash to hex
    for (int i = 0; i < SIZE_OF_SHA_256_HASH; i++) {
        uint8_t_to_hex(blockHeader.merkle_root_hash[i], output);
        output += 2;
    }

    // Convert time to hex
    uint32_t_to_hex(to_little_endian_32(blockHeader.time), output);
    output += 8;
    
    // Convert bits to hex
    uint32_t_to_hex(to_little_endian_32(blockHeader.bits), output);
    output += 8;
    
    // Convert nonce to hex
    uint32_t_to_hex(to_little_endian_32(blockHeader.nonce), output);
}
void calculate_target_from_bits(uint32_t bits, uint8_t target[SIZE_OF_SHA_256_HASH]) {
    //Formula : target = mantissa * 256 ( exponent -3 )
	
	uint8_t exponent = (bits >> 24) & 0xFF;	// Exponent = first byte .
    uint32_t mantissa = bits & 0xFFFFFF; // Mantissa = The three following bytes.
    // Clear the target array
    memset(target, 0, SIZE_OF_SHA_256_HASH);
	// Put the mantissa in first 3 bytes of the array
	for(int i = 0; i <3; i++){
		target[i] = ( mantissa >> (i * 8) ) & 0xFF;
	}
	// Shift the mantissa according to 256^( exponent-3 ). 
	// We will use memmove, it fits perfectly because we are shifting bytes.
	int shift = exponent -3;
	if( shift > 0){
		//Shift to the left by 8 * ( exponent - 3) = multiplying by 256^( exponent -3 )
		memmove(target + shift, target, SIZE_OF_SHA_256_HASH - shift);
		//Set the rest to 0xff
		memset(target,0xff,shift);  // not sure if 00 or ff ? logically ff
	}
	else if ( shift < 0 ){
		//Shift to the right by 8 * ( -exponent + 3) = dividing by 256^( -exponent +3 )
		memmove(target, target - shift, SIZE_OF_SHA_256_HASH + shift);
		//Set the rest to 0xff // not sure if 00 or ff ? logically ff
		memset(target + shift, 0xff, -shift);
	}
	//Do nothing if shift = 0. because we multiply by 256^0 which is 1.
}
int compare_hashes(const uint8_t *hash1, const uint8_t *hash2, size_t size) {
    for (size_t i = 31; i >= 0; i--) {
        if (hash1[i] < hash2[i]) return -1; // hash1 is less than hash2
        if (hash1[i] > hash2[i]) return 1;  // hash1 is greater than hash2
    }
    return 0; // hashes are equal
}
#ifdef DPU
uint32_t scan_hash(blockHeader bh, uint8_t target[SIZE_OF_SHA_256_HASH],uint32_t nonce_start,uint32_t nonce_end, uint32_t* dpu_found) {
    uint8_t hash[SIZE_OF_SHA_256_HASH];
    char concatenated_header[CONCAT_LENGTH];
    for (uint32_t nonce = bh.nonce + nonce_start ; nonce < nonce_end; nonce++) {
        bh.nonce = nonce; // Update nonce in block header
        concat_block_header(bh,concatenated_header);
        calc_sha_256(hash, concatenated_header, strlen(concatenated_header));
        calc_sha_256(hash,hash,SIZE_OF_SHA_256_HASH);
        if (compare_hashes(hash, target, SIZE_OF_SHA_256_HASH) < 0) {
            *dpu_found = 1;
            return nonce; 
            
        }
    }
    *dpu_found  = 0;
    return UINT32_MAX; // Return 0xffffffff if no valid hash is found within nonce range
}
#endif //DPU
