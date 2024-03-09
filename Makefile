HOST-CC           = gcc
HOST-CFLAGS       = -std=c99
HOST-LIBS         = `dpu-pkg-config --cflags --libs dpu`
DPU-CC            = dpu-upmem-dpurte-clang
NB_TASKLETS       ?= 1

all: clean dpu-miner host-miner

dpu-miner: src/dpu-miner.c include/sha-256.c include/blockHeader.c
	$(DPU-CC) -DNR_TASKLETS=$(NB_TASKLETS) -DDPU -o bin/dpu-miner src/dpu-miner.c include/sha-256.c include/blockHeader.c
host-miner: src/host-miner.c include/sha-256.c include/blockHeader.c include/hostTools.c
	$(HOST-CC) $(HOST-CFLAGS) -DHOST src/host-miner.c include/sha-256.c include/blockHeader.c include/hostTools.c -o bin/host-miner $(HOST-LIBS)

clean:
	rm -f bin/host-miner bin/dpu-miner