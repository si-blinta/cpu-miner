#include "../include/bitcoinRequest.h"

void serialize(const  blockHeader *data, char *buffer) {
    size_t offset = 0;
    memcpy(buffer + offset, &data->version, sizeof(data->version));
    offset += sizeof(data->version);
    memcpy(buffer + offset, &data->previous_hash, sizeof(data->previous_hash));
    offset += sizeof(data->previous_hash);
    memcpy(buffer + offset, &data->merkle_root_hash, sizeof(data->merkle_root_hash));
    offset += sizeof(data->merkle_root_hash);
    memcpy(buffer + offset, &data->time, sizeof(data->time));
    offset += sizeof(data->time);
    memcpy(buffer + offset, &data->bits, sizeof(data->bits));
    offset += sizeof(data->bits);
    memcpy(buffer + offset, &data->nonce, sizeof(data->nonce));
    offset += sizeof(data->nonce);
}


void deserialize(blockHeader *data, const char *buffer) {
    size_t offset = 0;
    memcpy(&data->version, buffer + offset, sizeof(data->version));
    offset += sizeof(data->version);
    memcpy(&data->previous_hash, buffer + offset, sizeof(data->previous_hash));
    offset += sizeof(data->previous_hash);
    memcpy(&data->merkle_root_hash, buffer + offset, sizeof(data->merkle_root_hash));
    offset += sizeof(data->merkle_root_hash);
    memcpy(&data->time, buffer + offset, sizeof(data->time));
    offset += sizeof(data->time);
    memcpy(&data->bits, buffer + offset, sizeof(data->bits));
    offset += sizeof(data->bits);
    memcpy(&data->nonce, buffer + offset, sizeof(data->nonce));
    offset += sizeof(data->nonce);

}
int get_block(const struct sockaddr_in* dest_addr, int sockfd){
    char buffer[BLOCK_HEADER_PACKET_SIZE] = {GET};
    if(sendto(sockfd, buffer, BLOCK_HEADER_PACKET_SIZE, 0,(struct sockaddr*) dest_addr, sizeof(*dest_addr))<0){
            perror("[get_block][sendto]");
            return -1;
    }
    return 0;
}

int send_block(const struct sockaddr_in* dest_addr, int sockfd,blockHeader* block_header, socklen_t len){
    char packet[BLOCK_HEADER_PACKET_SIZE];
    packet[0] = PUT;
    serialize(block_header,packet+1);
    if(sendto(sockfd, packet,BLOCK_HEADER_PACKET_SIZE,0,(struct sockaddr*) dest_addr, len)<0){
            perror("[send_block][sendto]");
            return -1;
    }
    return 0;
}
int verify_block(blockHeader block_header){
    uint8_t target[SIZE_OF_SHA_256_HASH];
    calculate_target_from_bits(block_header.bits,target);
    uint8_t concat_block[CONCAT_LENGTH];
    uint8_t block_hash[SIZE_OF_SHA_256_HASH];
    concat_block_header(block_header,concat_block);
    
    calc_sha_256(block_hash,concat_block,strlen(concat_block));
    calc_sha_256(block_hash,block_hash,SIZE_OF_SHA_256_HASH);

    if(compare_hashes(block_hash,target,SIZE_OF_SHA_256_HASH) < 0){
        printf("[SERVER] Block accepted \n[SERVER] Proof:\n");
        print_256_bits_integer(block_hash,"block hash");
        print_256_bits_integer(target,"target hash");
        return 0;
    }
    printf("[SERVER] Block rejected\n[SERVER] Proof:\n"); 
    print_256_bits_integer(block_hash,"block hash");
    print_256_bits_integer(target,"target hash");
    return -1;
   

}