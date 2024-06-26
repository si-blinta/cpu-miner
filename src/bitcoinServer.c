#include "../include/bitcoinRequest.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
int init_server(int* sockfd,struct sockaddr_in server_addr, int port){
    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(*sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("Bitcoin Server listening for requests\n");
}


int main(int argc, char *argv[]) {

    int sockfd;
    struct sockaddr_in server_addr,client_addr;
    socklen_t len = sizeof(client_addr);
    char buffer[BLOCK_HEADER_PACKET_SIZE];
    init_server(&sockfd,server_addr,BITCOIN_PORT);
    blockHeader block;
    while (1) {
        
        int bytes_received = recvfrom(sockfd, buffer, BLOCK_HEADER_PACKET_SIZE , 0, (struct sockaddr *)&client_addr, &len);
          if(bytes_received == -1){
            perror("[SERVER][recvfrom]");
            exit(1);
          }  
        switch (buffer[0])
        {
        case GET:
            printf("[SERVER] Received request block\n");
            generate_block_header(&block);
            printf("[SERVER] Block generated\n");
            send_block(&client_addr,sockfd,&block,len);
            printf("[SERVER] Block sent\n");
            break;
        
        case PUT:
            printf("[SERVER] Received a block\n");
            deserialize(&block,buffer+1);
            printf("[SERVER] Verifying the block\n");
            verify_block(block);
            break;
        default:
            printf("[SERVER] Unexpected block\n");
            break;
        }
    }
    return 0;
}
