#include "../include/hostTools.h"
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

static void HOST_TOOLS_test(uint32_t nb_dpus_p, uint32_t nb_tasklets_p, uint32_t nb_hashes_p, uint8_t type_p, char* path){
  FILE* performance_file = fopen(path,"a+");
  if(performance_file == NULL){
    perror("[fopen]\n");
    exit (EXIT_FAILURE);
  }
  uint32_t nb_dpus     = nb_dpus_p;
  uint32_t nb_tasklets = nb_tasklets_p;
  uint32_t nb_hashes   = nb_hashes_p;
  uint32_t type        = type_p;
  struct dpu_set_t set, dpu;
  HOST_TOOLS_allocate_dpus(&set,&nb_dpus);
  HOST_TOOLS_compile(nb_tasklets);
  DPU_ASSERT(dpu_load(set,DPU_BINARY,NULL));
  
  blockHeader bh;
  uint8_t target[SIZE_OF_SHA_256_HASH];
  
  //Generating block header and calculating target hash
  generate_block_header(&bh);                 
  calculate_target_from_bits(bh.bits,target); 
  /**
   * Broadcasting the blockHeader , the target hash and number of dpus allocated to all DPUs
  */
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_block_header", 0,&bh,sizeof(bh), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_target", 0,target,SIZE_OF_SHA_256_HASH * sizeof(uint8_t), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_nb", 0,&nb_dpus,sizeof(nb_dpus), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_nb_hashes", 0,&nb_hashes,sizeof(nb_hashes), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_test_type", 0,&type,sizeof(type), DPU_XFER_DEFAULT));
  /**
   * Sending IDs to each DPU.
  */
  HOST_TOOLS_send_id(set);
  /**
   * Launching in Synchronous way.
  */
  clock_t start,end;
  double duration;
  start = clock();
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  end = clock();
  duration = ((double) (end - start)) / CLOCKS_PER_SEC;
  switch (type)
  {
  case DPU_SOL_TEST:
    printf("Did %u solutions in %lf seconds  => Sol/s = %lf\n",nb_hashes,duration, (double) nb_hashes/duration);
    fprintf(performance_file,"%d,%lf\n",nb_dpus,(double) nb_hashes/duration); 
    break;
  case TASKLET_SOL_TEST:
    printf("Did %u solutions in %lf seconds  => Sol/s = %lf\n",nb_hashes,duration, (double) nb_hashes/duration);
    fprintf(performance_file,"%d,%lf\n",nb_tasklets,(double) nb_hashes/duration); 
    break;
  case DPU_HASHRATE_TEST:
    printf("Did %u hashes in %lf seconds  => Hashes/s = %lf\n",nb_hashes,duration, (double) nb_hashes/duration);
    fprintf(performance_file,"%d,%lf\n",nb_dpus,(double) nb_hashes/duration); 
    break;
  case TASKLET_HASHRATE_TEST:
    printf("Did %u hashes in %lf seconds  => Hashes/s = %lf\n",nb_hashes,duration, (double) nb_hashes/duration);
    fprintf(performance_file,"%d,%lf\n",nb_tasklets,(double) nb_hashes/duration);  
    break;
  default:
    break;
  }

  fclose(performance_file);
  DPU_ASSERT(dpu_free(set));
}

void HOST_TOOLS_dpu_test(char* path, uint8_t type){
  if(type == DPU_SOL_TEST){
    printf("starting solution test for dpu\n");
    for(int i = 50; i < 1150; i+=50){
      HOST_TOOLS_test(i,11,10000*i,DPU_SOL_TEST,path);
    }
    HOST_TOOLS_test(DPU_ALLOCATE_ALL,11,10000*1000,DPU_SOL_TEST,path);
  }
  else if(type == DPU_HASHRATE_TEST){
    printf("starting hash rate test for dpu\n");
    for(int i = 50; i < 1150; i+=50){
      HOST_TOOLS_test(i,11,10000*i,DPU_HASHRATE_TEST,path);
    }
    HOST_TOOLS_test(DPU_ALLOCATE_ALL,11,10000*1000,DPU_HASHRATE_TEST,path);
  }
  
}

void HOST_TOOLS_tasklet_test(char* path,uint8_t type){
  if(type == TASKLET_SOL_TEST){
    printf("starting solution test for tasklets\n");
    for(int i = 1; i < 25; i++){
      HOST_TOOLS_test(1,i,1000*i,TASKLET_SOL_TEST,path);
    }
  }
  else if(type == TASKLET_HASHRATE_TEST){
    printf("starting hash rate test for tasklets\n");
    for(int i = 1; i < 25; i++){
      HOST_TOOLS_test(1,i,1000*i,TASKLET_HASHRATE_TEST,path);
    }
  }
}