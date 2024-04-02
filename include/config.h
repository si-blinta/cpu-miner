#ifndef CONFIG_H
#define CONFIG_H
#define HOST_DEBUG 0                //LOGS for host
#define DPU_DEBUG  0                //LOGS for dpu
#define DPU_BINARY "bin/dpu-miner"  //Binary file for dpu
#define BLOCK_HEADER_PACKET_SIZE 81 
#define BITCOIN_PORT 9999           //Server port
#define CONCAT_LENGTH 161          
#define TRAILING_ZEROS(x) ((x)+6)/2
#define LOCALHOST "127.0.0.1"
#define DIFFICULTY  2
#define OPTIMISED   1               // 0 to use the sha256 given in the project
                                    // 1 to use the other sha256 ( more performance )

/**
 * Difficulty 0 to 3 it represent how difficult are the target hashes.
 * */              
#endif //CONFIG_H