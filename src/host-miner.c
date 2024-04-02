#include "../include/blockHeader.h"
#include "../include/hostTools.h"


int main(int argc, char** argv) {
  srand(time(NULL));
  /**
   * Connection variables
  */
  int sockfd;
  struct sockaddr_in server_addr;
  char buffer[BLOCK_HEADER_PACKET_SIZE];

  /**
   * Mining variables
   * 
  */
  blockHeader bh;
  uint32_t nb_dpus;
  uint8_t nb_tasklets;
  uint32_t nb_boots;
  uint32_t nb_blocks_to_mine;

  HOST_TOOLS_parse_args(argc,argv,&nb_dpus,&nb_tasklets,&nb_boots,&nb_blocks_to_mine);
  
  /**
   * Allocation of DPUS and compilation and loading of the binary.
  */
  struct dpu_set_t set, dpu;
  HOST_TOOLS_allocate_dpus(&set,&nb_dpus);
  HOST_TOOLS_compile(nb_tasklets);
  DPU_ASSERT(dpu_load(set,DPU_BINARY,NULL));
  
  /**
   * Connection to the "bitcoin server"
  */

  if(HOST_TOOLS_connect(LOCALHOST,BITCOIN_PORT,&server_addr,&sockfd) == -1){
    fprintf(stderr,"Exiting miner\n");
    DPU_ASSERT(dpu_free(set));
    return -1;
  }

/**
 * Mining process
 * 
*/

  HOST_TOOLS_mine(server_addr,sockfd,set,nb_dpus,nb_blocks_to_mine,nb_boots);
  /**
   * Freeing the set of dpus.
  */

  DPU_ASSERT(dpu_free(set));

    
  return 0;
    
  }
