HOST-CC           = gcc
HOST-CFLAGS       = -std=c99
HOST-LIBS         = `dpu-pkg-config --cflags --libs dpu`
DPU-CC            = dpu-upmem-dpurte-clang

all: clean dpu-miner host-miner

dpu-miner: src/dpu-miner.c include/sha-256.c include/blockHeader.c
	$(DPU-CC) -DNR_TASKLETS=24 -DDPU -o bin/dpu-miner src/dpu-miner.c include/sha-256.c include/blockHeader.c
host-miner: src/host-miner.c include/sha-256.c include/blockHeader.c
	$(HOST-CC) $(HOST-CFLAGS) -DHOST src/host-miner.c include/sha-256.c include/blockHeader.c -o bin/host-miner $(HOST-LIBS)

clean:
	rm -f bin/host-miner bin/dpu-miner