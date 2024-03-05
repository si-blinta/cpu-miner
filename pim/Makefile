HOST-CC = gcc
HOST-CFLAGS = -std=c99
HOST-LIBS = `dpu-pkg-config --cflags --libs dpu`
DPU-CC  = dpu-upmem-dpurte-clang
all: dpu-miner host-miner
	$(DPU-CC) -o dpu-miner dpu-miner.c sha-256-dpu.c
	$(HOST-CC) $(HOST-CFLAGS) host-miner.c sha-256.c -o host-miner $(HOST-LIBS)
dpu-miner: dpu-miner.c sha-256-dpu.c
	$(DPU-CC) -o dpu-miner dpu-miner.c sha-256-dpu.c
host-miner: host-miner.c sha-256.c
	$(HOST-CC) $(HOST-CFLAGS) host-miner.c sha-256.c -o host-miner $(HOST-LIBS)

clean:
	rm -f hash
	rm -f dpu-miner