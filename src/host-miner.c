#include "../include/blockHeader.h"
#include "../include/hostTools.h"


int main(int argc, char** argv) {
  srand(time(NULL));
  int sockfd;
  struct sockaddr_in server_addr;
  char buffer[BLOCK_HEADER_PACKET_SIZE];

  blockHeader bh;
  uint32_t nb_dpus;
  uint8_t nb_tasklets;
  uint32_t nb_boots;
  HOST_TOOLS_parse_args(argc,argv,&nb_dpus,&nb_tasklets,&nb_boots);
  struct dpu_set_t set, dpu;
  HOST_TOOLS_allocate_dpus(&set,&nb_dpus);
  HOST_TOOLS_compile(nb_tasklets);
  DPU_ASSERT(dpu_load(set,DPU_BINARY,NULL));
  uint8_t target[SIZE_OF_SHA_256_HASH];
  HOST_TOOLS_connect(LOCALHOST,BITCOIN_PORT,&server_addr,&sockfd);
  HOST_TOOLS_mine(server_addr,sockfd,set,nb_dpus,2,nb_boots);
  DPU_ASSERT(dpu_free(set));

    
  
    
  }
