#include "../include/hostTools.h"
void HOST_TOOLS_parse_args(int argc, char** argv, uint32_t* nb_dpus,uint8_t* nb_tasklets,uint32_t* nb_boots){
    if(argc < 4){
    fprintf(stderr,"Usage : %s [nb_dpus] [nb_tasklets] [nb_boots]\n",argv[0]);
    fprintf(stdout,"0 if you want to allocate all available DPUs\n");
    exit(EXIT_FAILURE);
  }
  if(atoi(argv[2]) <= 0 ||  atoi(argv[2]) > 24 || atoi(argv[1]) < 0 || atoi(argv[1]) > 1280 || atoi(argv[3]) < 0){
    fprintf(stderr,"1 <= nb_tasklets <= 24 | 0 <= nb_dpus <= 1280 | nb_boots > 1 \n");
    exit(EXIT_FAILURE);
  }
  *nb_dpus     = (uint32_t)atoi(argv[1]);
  *nb_tasklets = (uint32_t)atoi(argv[2]);
  *nb_boots = (uint32_t)atoi(argv[3]);
  printf("BOOOOOOTS = %u\n",*nb_boots);
  if(  *nb_dpus == 0 )
    *nb_dpus = DPU_ALLOCATE_ALL;
}


void HOST_TOOLS_allocate_dpus(struct dpu_set_t* set,uint32_t* nb_dpus){
    DPU_ASSERT(dpu_alloc(*nb_dpus, NULL, set));
    DPU_ASSERT(dpu_get_nr_dpus(*set,nb_dpus));
    printf("ALLOCATED : %u DPUs \n",*nb_dpus);
}

void HOST_TOOLS_send_id(struct dpu_set_t set){
  struct dpu_set_t dpu;
  uint32_t id = 0;
  DPU_FOREACH(set,dpu,id){
      DPU_ASSERT(dpu_copy_to(dpu, "dpu_id", 0,&id,sizeof(uint32_t)));
  }
}

void HOST_TOOLS_compile(uint8_t nb_tasklets){
    char command[100];
    sprintf(command,"make dpu-miner NB_TASKLETS=%d", nb_tasklets);
    system(command);
}

uint32_t HOST_TOOLS_mine_stop_repeat( struct dpu_set_t set,blockHeader bh,uint8_t target[SIZE_OF_SHA_256_HASH],uint32_t nb_dpus,uint32_t nb_boots, uint32_t* host_found){
    printf("Mining with number of boots = %u\n",nb_boots);
    struct dpu_set_t dpu;
    uint32_t golden_nonce = UINT32_MAX;
    uint32_t found        = 0;
    DPU_ASSERT(dpu_broadcast_to(set, "dpu_block_header", 0,&bh,sizeof(bh), DPU_XFER_DEFAULT));
    DPU_ASSERT(dpu_broadcast_to(set, "dpu_target", 0,target,SIZE_OF_SHA_256_HASH * sizeof(uint8_t), DPU_XFER_DEFAULT));
    DPU_ASSERT(dpu_broadcast_to(set, "dpu_nb", 0,&nb_dpus,sizeof(nb_dpus), DPU_XFER_DEFAULT));
    DPU_ASSERT(dpu_broadcast_to(set, "dpu_nb_boots", 0,&nb_boots,sizeof(nb_boots), DPU_XFER_DEFAULT));
    DPU_ASSERT(dpu_broadcast_to(set, "dpu_nonce", 0,&golden_nonce,sizeof(golden_nonce), DPU_XFER_DEFAULT));
    DPU_ASSERT(dpu_broadcast_to(set, "dpu_found", 0,&found,sizeof(found), DPU_XFER_DEFAULT));
    HOST_TOOLS_send_id(set);
    for(uint32_t i = 0; i < nb_boots ; i ++){
#if HOST_DEBUG
            printf("BOOT #%d little endian nonce = %08x : found = %d\n",i,to_little_endian_32(golden_nonce),found);
#endif//HOST_DEBUG         
        DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
        DPU_FOREACH(set, dpu) {
            DPU_ASSERT(dpu_copy_from(dpu,"dpu_found",0,&found,sizeof(uint32_t)));
            if(found){
#if DPU_DEBUG
                DPU_ASSERT(dpu_log_read(dpu,stdout));
#endif//DPU_DEBUG                
                DPU_ASSERT(dpu_copy_from(dpu,"dpu_nonce",0,&golden_nonce,sizeof(uint32_t)));  
                goto return_success;
            }
        }
    }
    //DPU_ASSERT(dpu_free(set));
    *host_found = 0;
    return UINT32_MAX;

return_success:
    //DPU_ASSERT(dpu_free(set));
    *host_found = 1;
    return golden_nonce;
}
int HOST_TOOLS_connect(const char* server_ip, int server_port,struct sockaddr_in* server_addr,int* sockfd) {

    if ((*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("[socket]");
        return -1;
    }
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(server_port);
    server_addr->sin_addr.s_addr = inet_addr(server_ip); 
    printf("Connected to bitcoin server\n");
    return 0;
}

void HOST_TOOLS_mine(struct sockaddr_in server_addr,int sockfd,struct dpu_set_t set,uint32_t nb_dpus,
                    size_t number_of_blocks_to_mine,uint32_t nb_boots){
  blockHeader bh;
  uint32_t golden_nonce;
  uint32_t found = 0;
  uint8_t target[SIZE_OF_SHA_256_HASH];
  char buffer[BLOCK_HEADER_PACKET_SIZE];
  for(size_t i = 0 ; i < number_of_blocks_to_mine; i++){

    get_block(&server_addr,sockfd);
    recvfrom(sockfd,buffer,BLOCK_HEADER_PACKET_SIZE,0,NULL,NULL);
    deserialize(&bh,buffer+1);
    calculate_target_from_bits(bh.bits,target); 
#if HOST_DEBUG    
    print_256_bits_integer(target,"Target Hash");
    print_block_header(bh);
#endif//HOST_DEBUG
    golden_nonce = HOST_TOOLS_mine_stop_repeat(set,bh,target,nb_dpus,nb_boots,&found);
    if(found){
        bh.nonce = golden_nonce;
        send_block(&server_addr,sockfd,&bh,sizeof(server_addr));
    }
  }

}