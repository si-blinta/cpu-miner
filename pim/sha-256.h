#ifndef SHA_256_H
#define SHA_256_H
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#define TRAILING_ZEROS(x) ((x)+6)/2
#define EASY
/*
 * @brief Size of the SHA-256 sum. This times eight is 256 bits.
 */
#define SIZE_OF_SHA_256_HASH 32

/*
 * @brief Size of the chunks used for the calculations.
 *
 * @note This should mostly be ignored by the user, although when using the streaming API, it has an impact for
 * performance. Add chunks whose size is a multiple of this, and you will avoid a lot of superfluous copying in RAM!
 */
#define SIZE_OF_SHA_256_CHUNK 64

/*
 * @brief The opaque SHA-256 type, that should be instantiated when using the streaming API.
 *
 * @note Although the details are exposed here, in order to make instantiation easy, you should refrain from directly
 * accessing the fields, as they may change in the future.
 */
struct Sha_256 {
	uint8_t *hash;
	uint8_t chunk[SIZE_OF_SHA_256_CHUNK];
	uint8_t *chunk_pos;
	size_t space_left;
	uint64_t total_len;
	uint32_t h[8];
};
/*
 * @brief Structure representing a blockchain block header.
 */
typedef struct {
    int32_t version;                // Version of the block
    uint8_t previous_hash[32];      // Hash of the previous block in the chain
    uint8_t merkle_root_hash[32];   // Root hash of the Merkle tree of transactions in this block
    uint32_t time;                  // Timestamp for when this block was created
    uint32_t bits;                 // Difficulty
    uint32_t nonce;                 // Counter used for proof-of-work algorithm
} blockHeader;

typedef struct {
    blockHeader bh;
    uint32_t nonceMax;
    uint32_t* result;
} ThreadData;


/*
 * @brief The simple SHA-256 calculation function.
 * @param hash Hash array, where the result is delivered.
 * @param input Pointer to the data the hash shall be calculated on.
 * @param len Length of the input data, in byte.
 *
 * @note If all of the data you are calculating the hash value on is available in a contiguous buffer in memory, this is
 * the function you should use.
 *
 * @note If either of the passed pointers is NULL, the results are unpredictable.
 *
 * @note See note about maximum data length for sha_256_write, as it applies for this function's len argument too.
 */
void calc_sha_256(uint8_t hash[SIZE_OF_SHA_256_HASH], const void *input, size_t len);

/*
 * @brief Initialize a SHA-256 streaming calculation.
 * @param sha_256 A pointer to a SHA-256 structure.
 * @param hash Hash array, where the result will be delivered.
 *
 * @note If all of the data you are calculating the hash value on is not available in a contiguous buffer in memory,
 * this is where you should start. Instantiate a SHA-256 structure, for instance by simply declaring it locally, make
 * your hash buffer available, and invoke this function. Once a SHA-256 hash has been calculated (see further below) a
 * SHA-256 structure can be initialized again for the next calculation.
 *
 * @note If either of the passed pointers is NULL, the results are unpredictable.
 */
void sha_256_init(struct Sha_256 *sha_256, uint8_t hash[SIZE_OF_SHA_256_HASH]);

/*
 * @brief Stream more input data for an on-going SHA-256 calculation.
 * @param sha_256 A pointer to a previously initialized SHA-256 structure.
 * @param data Pointer to the data to be added to the calculation.
 * @param len Length of the data to add, in byte.
 *
 * @note This function may be invoked an arbitrary number of times between initialization and closing, but the maximum
 * data length is limited by the SHA-256 algorithm: the total number of bits (i.e. the total number of bytes times
 * eight) must be representable by a 64-bit unsigned integer. While that is not a practical limitation, the results are
 * unpredictable if that limit is exceeded.
 *
 * @note This function may be invoked on empty data (zero length), although that obviously will not add any data.
 *
 * @note If either of the passed pointers is NULL, the results are unpredictable.
 */
void sha_256_write(struct Sha_256 *sha_256, const void *data, size_t len);

/*
 * @brief Conclude a SHA-256 streaming calculation, making the hash value available.
 * @param sha_256 A pointer to a previously initialized SHA-256 structure.
 * @return Pointer to the hash array, where the result is delivered.
 *
 * @note After this function has been invoked, the result is available in the hash buffer that initially was provided. A
 * pointer to the hash value is returned for convenience, but you should feel free to ignore it: it is simply a pointer
 * to the first byte of your initially provided hash array.
 *
 * @note If the passed pointer is NULL, the results are unpredictable.
 *
 * @note Invoking this function for a calculation with no data (the writing function has never been invoked, or it only
 * has been invoked with empty data) is legal. It will calculate the SHA-256 value of the empty string.
 */
uint8_t *sha_256_close(struct Sha_256 *sha_256);
/*
 * @brief Function to calculate the target hash from the compact representation nBits.
 * @param nBits Compact representation of the target hash.
 * @param target Buffer to store the calculated target hash.
 */
void calculate_target_from_nbits(uint32_t nBits, uint8_t target[32]);

/*
 * @brief Function to generate a random hash for testing purposes.
 * @param str Pointer to the buffer where the random hash will be stored.
 * @param size Size of the buffer.
 */
void generate_random_hash(char *str, size_t size);

/*
 * @brief Function to generate a random block header for testing purposes.
 * @param block_header Pointer to the block header structure to be populated.
 */
void generate_block_header(blockHeader *block_header);

/*
 * @brief Function to print out the contents of a block header.
 * @param blockHeader The block header to print.
 */
void print_block_header(const blockHeader blockHeader);

/*
 * @brief Function to concatenate the components of a block header into a single string.
 * @param blockHeader The block header to concatenate.
 * @return Pointer to the concatenated string.
 */
char* concat_block_header(const blockHeader blockHeader);

/*
 * @brief Function to compare two hashes byte by byte.
 * @param hash1 First hash for comparison.
 * @param hash2 Second hash for comparison.
 * @param size Size of the hash arrays.
 * @return -1 if hash1 < hash2, 1 if hash1 > hash2, 0 if equal.
 */
int compare_hashes(const uint8_t *hash1, const uint8_t *hash2, size_t size);

/*
 * @brief Function to test hashing performance by finding a hash less than a target
 * within a given nonce range and measuring the speed of hashes per second.
 * @param bh The block header for the hashing test.
 * @param nonceMax The maximum nonce value to be tested.
 * @return The nonce value that satisfies the condition, or -1 if not found.
 */
uint32_t scan_hash_test(blockHeader bh, uint32_t nonceMax);




#endif
