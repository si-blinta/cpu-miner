#include "../include/blockHeader.h"
#include "../include/hostTools.h"



int main(int argc, char** argv) {
  srand(time(NULL));
  if(argc < 2){
     fprintf(stderr,"Usage %s [dpu] or [tasklet] or [all]\n",argv[0]);
    return -1;
  }
  if(!strcmp(argv[1],"dpu")){ //DPU TEST
    HOST_TOOLS_dpu_test("csv/dpu.csv");
    printf("%s test is done !\n",argv[1]);
    printf("check csv directory\n");
    return 0;
  }
  else if(!strcmp(argv[1],"tasklet")){  //TASKLET TEST
    HOST_TOOLS_tasklet_test("csv/tasklet.csv");
    printf("%s test is done !\n",argv[1]);
    printf("check csv directory\n");
    return 0;
  }
  else if(!strcmp(argv[1],"all")){    //BOTH TESTS
    HOST_TOOLS_tasklet_test("csv/tasklet.csv");
    HOST_TOOLS_dpu_test("csv/dpu.csv");
    printf("%s test is done !\n",argv[1]);
    printf("check csv directory\n");
    return 0;
  }
  fprintf(stderr,"Usage %s [dpu] or [tasklet] or [all]\n",argv[0]);
  return -1;
 
}