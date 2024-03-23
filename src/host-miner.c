#include "../include/blockHeader.h"
#include "../include/hostTools.h"



int main(int argc, char** argv) {
  srand(time(NULL));
  if(argc < 3){
     fprintf(stderr,"Usage %s [dpu] or [tasklet] or [all] then [hashrate] or [solution]\n",argv[0]);
    return -1;
  }
  if(!strcmp(argv[1],"dpu")){ //DPU TEST
    if(!strcmp(argv[2],"solution")){
      HOST_TOOLS_dpu_test("csv/dpu-solution.csv",DPU_SOL_TEST);
      printf("%s solution test is done !\n",argv[1]);
      printf("check csv directory\n");
    }
    else if(!strcmp(argv[2],"hashrate")){
      HOST_TOOLS_dpu_test("csv/dpu-hashrate.csv",DPU_HASHRATE_TEST);
      printf("%s hashrate test is done !\n",argv[1]);
      printf("check csv directory\n");
    }
    else
      goto out_failure;
    return 0;
  }
  else if(!strcmp(argv[1],"tasklet")){  //TASKLET TEST
    if(!strcmp(argv[2],"solution")){
      HOST_TOOLS_tasklet_test("csv/tasklet-solution.csv",TASKLET_SOL_TEST);
      printf("%s solution test is done !\n",argv[1]);
      printf("check csv directory\n");
    }
    else if(!strcmp(argv[2],"hashrate")){
      HOST_TOOLS_tasklet_test("csv/tasklet-hashrate.csv",TASKLET_HASHRATE_TEST);
      printf("%s hashrate test is done !\n",argv[1]);
      printf("check csv directory\n");
    }
    else
      goto out_failure;
    return 0;
  }
  else if(!strcmp(argv[1],"all")){    //BOTH TESTS
    if(!strcmp(argv[2],"solution")){
      HOST_TOOLS_tasklet_test("csv/tasklet-solution.csv",DPU_SOL_TEST);
      HOST_TOOLS_dpu_test("csv/dpu-solution.csv",TASKLET_SOL_TEST);
      printf("%s solution test is done !\n",argv[1]);
      printf("check csv directory\n");
    }
    else if(!strcmp(argv[2],"hashrate")){
      HOST_TOOLS_tasklet_test("csv/tasklet-hashrate.csv",DPU_HASHRATE_TEST);
      HOST_TOOLS_dpu_test("csv/dpu-hashrate.csv",TASKLET_HASHRATE_TEST);
      printf("%s hashrate test is done !\n",argv[1]);
      printf("check csv directory\n");
    }
    else
      goto out_failure;
    return 0;
  }
out_failure:
  
  fprintf(stderr,"Usage %s [dpu] or [tasklet] or [all] then [hashrate] or [solution]\n",argv[0]);
  return -1;
 
}