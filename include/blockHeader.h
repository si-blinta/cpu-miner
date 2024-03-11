#ifndef BLOCK_HEADER_H
#define BLOCK_HEADER_H
#include "sha-256.h"
#ifdef HOST
#include <time.h>
//#include <curl/curl.h>
#endif //HOST
#ifdef DPU
#include <perfcounter.h>
#endif  //DPU
#define CONCAT_LENGTH 161           // Size of concatenated blockHeader. : // 8 characters for each 32-bit int (hex) + 64 characters for each hash (hex) + null terminator
                                    // 8 + 64 + 64 + 8 + 8 + 8 + 1 = 161.
#define TRAILING_ZEROS(x) ((x)+6)/2
#define EASY
typedef struct {
    int32_t version;                // Version of the block
    uint8_t previous_hash[32];      // Hash of the previous block in the chain
    uint8_t merkle_root_hash[32];   // Root hash of the Merkle tree of transactions in this block
    uint32_t time;                  // Timestamp for when this block was created
    uint32_t bits;                  // Difficulty
    uint32_t nonce;                 // Counter used for proof-of-work algorithm
} blockHeader;


/**
 * @brief Transform a 4 byte int to little endian.
 * @param value 4 byte int.
 * @return Value stored in little endian.
*/
uint32_t to_little_endian_32(uint32_t value);

/**
 * @brief Stores 4 bytes integer in a buffer in a hexadicmal format.(used because we cannot use sprintf in dpu code).
 * @param value 4 bytes integer to store.
 * @param buffer The buffer to store in the result.
*/
void uint32_t_to_hex(uint32_t value, char* buffer);

/**
 * @brief Stores 1 byte integer in a buffer in a hexadicmal format.(used because we cannot use sprintf in dpu code).
 * @param value 1 bytes integer to store.
 * @param buffer The buffer to store in the result.
*/
void uint8_t_to_hex(uint8_t value, char* buffer);
#ifdef HOST
/**
 * @brief Generates a random block header for testing purposes. ( not totally random )
 * @param block_header Pointer to the block header structure.
*/
void generate_block_header(blockHeader *block_header);
#endif //HOST
/**
 * @brief Prints block header contents in Big / Little endian.
 * @param block_header The block header to print.
*/
void print_block_header(const blockHeader block_header) ;

/**
 * @brief Prints a 256 bits integer in Big / Little endian.
 * @param value The 256 bits integer to print.
 * @param name   The name of the integer.
*/
void print_256_bits_integer(const uint8_t value[SIZE_OF_SHA_256_HASH],const char* name);

/**
 * @brief Concatenates block header contents into one string in little endian.
 *  stores the result in ouput.
 * @param block_header The block header to concatenate.
 * @param output       The output buffer.
*/
void concat_block_header(const blockHeader block_header,char output[CONCAT_LENGTH]);

/**
 * @brief Calculates the target hash from the compact representation bits.
 * @param bits Compact representation of the target hash.
 * @param target Buffer to store the calculated target hash.
 */
void calculate_target_from_bits(uint32_t bits, uint8_t target[SIZE_OF_SHA_256_HASH]);

/**
 * @brief Compares two hashes byte by byte.
 * @param hash1 First hash for comparison.
 * @param hash2 Second hash for comparison.
 * @param size Size of the hash arrays.
 * @return -1 if hash1 < hash2, 1 if hash1 > hash2, 0 if equal.
 */
int compare_hashes(const uint8_t *hash1, const uint8_t *hash2, size_t size);

/**
 * @brief test hashing performance by doing the same operations as if we
 * want to mine a block. The function returns only if the nonce_end is reached.
 * @param bh The block header for the hashing test.
 * @param target The target hash.
 * @param nonce_start The first nonce value to be tested. 
 * @param nonce_end The maximum nonce value to be tested.
 * @return The number of hashes done by a tasklet.
 */
uint32_t scan_hash_test(blockHeader bh, uint8_t target[SIZE_OF_SHA_256_HASH],uint32_t nonce_start,uint32_t nonce_end);

/**
 * @brief Mining function, trying a new nonce in every iteration to find the target hash.
 * @param bh The block header for the hashing test.
 * @param target The target hash.
 * @param nonce_start The first nonce value to be tested. 
 * @param nonce_end The maximum nonce value to be tested.
 * @return The nonce value that satisfies the condition, or 0xffffffff if not found.
 */
uint32_t scan_hash(blockHeader bh, uint8_t target[SIZE_OF_SHA_256_HASH],uint32_t nonce_start,uint32_t nonce_end);

/**
 *@brief This function will test if out double sha256 is working fine.By comparing the results
 *       with the results on this website : https://emn178.github.io/online-tools/double_sha256
 * @param input The String to hash.
*/

int unit_test_double_sha256(char* input);


#endif //BLOCK_HEADER_H