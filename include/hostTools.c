#include "hostTools.h"
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
    printf("Compiled dpu program with %u tasklets\n",nb_tasklets);
}