#ifndef CONFIG_H
#define CONFIG_H
#define HOST_DEBUG 0
#define DPU_DEBUG  0
#define DPU_BINARY "bin/dpu-miner"
#define BLOCK_HEADER_PACKET_SIZE 81
#define BITCOIN_PORT 8080
#define BUFFER_SIZE 1024
#define CONCAT_LENGTH 161          
#define TRAILING_ZEROS(x) ((x)+6)/2
#define LOCALHOST "127.0.0.1"
#define DIFFICULTY  2
/**
 * Difficulty 0 to 3 it represent how difficult are the target hashes.
 * */              
#endif //CONFIG_H