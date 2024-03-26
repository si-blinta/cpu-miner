HOST-CC           = gcc
HOST-CFLAGS       = -std=c99
HOST-LIBS         = `dpu-pkg-config --cflags --libs dpu`
DPU-CC            = dpu-upmem-dpurte-clang
NB_TASKLETS       ?= 1

all: clean dpu-miner host-miner server

dpu-miner: src/dpu-miner.c src/sha-256.c src/blockHeader.c
	@$(DPU-CC) -DNR_TASKLETS=$(NB_TASKLETS) -DDPU -o bin/dpu-miner src/dpu-miner.c src/sha-256.c src/blockHeader.c
	@echo "Compiled DPU with $(NB_TASKLETS) Tasklets"
host-miner: src/host-miner.c src/sha-256.c src/blockHeader.c src/hostTools.c src/bitcoinRequest.c
	@$(HOST-CC) $(HOST-CFLAGS) src/host-miner.c src/sha-256.c src/blockHeader.c src/hostTools.c src/bitcoinRequest.c -o bin/host-miner $(HOST-LIBS)
	@echo "Compiled HOST"

server: src/bitcoinServer.c src/bitcoinRequest.c src/blockHeader.c src/sha-256.c
	@@$(HOST-CC) $(HOST-CFLAGS) src/bitcoinServer.c src/bitcoinRequest.c src/blockHeader.c src/sha-256.c -o bin/server
	@echo "Compiled server"
	
clean:
	@rm -f bin/host-miner bin/dpu-miner
	@echo "Cleaned"