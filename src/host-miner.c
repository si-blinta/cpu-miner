#include "../include/blockHeader.h"
#include "../include/hostTools.h"
#include <time.h>
#define TASKLET_TEST_PATH "csv/hashrate_tasklets.csv"
#define DPU_TEST_PATH     "csv/hashrate_dpus1.csv"
#ifndef DPU_BINARY
#define DPU_BINARY "bin/dpu-miner"
#endif


/**
 * Questions:
 *          - How can i interrupt Dpus.
 *          - How can i reset the ressources in DPUS. ( is it needed in our case ? ) 
 *          - How can i wait for one dpu to end.      ( multi threading ? )
*/



int main(int argc, char** argv) {
  srand(time(NULL));
  if(argc < 4){
    fprintf(stderr,"Usage : %s [nb_dpus] [nb_tasklets] [nb_hashes]\n",argv[0]);
    fprintf(stdout,"0 if you want to allocate all available DPUs\n");
    exit(EXIT_FAILURE);
  }
  if(atoi(argv[2]) <= 0 ||  atoi(argv[2]) > 24 || atoi(argv[1]) < 0 || atoi(argv[1]) > 1280 || atoi(argv[3]) <= 0){
    fprintf(stderr,"1 <= nb_tasklets <= 24  0 <= nb_dpus <= 1280  nb_hashes >= 0\n");
    exit(EXIT_FAILURE);
  }
  uint32_t nb_dpus     = atoi(argv[1]);
  uint8_t  nb_tasklets = atoi(argv[2]);
  uint32_t nb_hashes = atoi(argv[3]);
  if(  nb_dpus == 0 )
    nb_dpus = DPU_ALLOCATE_ALL;
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
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_target", 0,&target,sizeof(target), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_nb", 0,&nb_dpus,sizeof(nb_dpus), DPU_XFER_DEFAULT));
  DPU_ASSERT(dpu_broadcast_to(set, "dpu_nb_hashes", 0,&nb_hashes,sizeof(nb_hashes), DPU_XFER_DEFAULT));
  /**
   * Sending IDs to each DPU.
  */
  HOST_TOOLS_send_id(set);
  /**
   * Launching in Synchronous way.
  */
  time_t start,end;
  double duration;
  time(&start);
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));
  time(&end);
  duration = difftime(end,start);
  printf("Did %u hashes in %lf seconds  => Hashrate = %lf\n",nb_hashes,duration, (double) nb_hashes/duration);
  FILE* performance_file = fopen(DPU_TEST_PATH,"a+");
  //fprintf(performance_file,"%d;%lf\n",nb_tasklets,(double) nb_hashes/duration); //for taskelts
  fprintf(performance_file,"%d;%lf\n",nb_dpus,(double) nb_hashes/duration);  // for dpus
  fclose(performance_file);
  DPU_ASSERT(dpu_free(set));
}