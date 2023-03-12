#ifndef HASH_H
#define HASH_H


#ifdef __cplusplus
#include <cstddef>
#include <cstdint>
	namespace {
		// type for function
		using hash_function_t = uint64_t (*)(uint64_t const *, size_t);
	}
extern "C" {
    namespace jnp1 {
#else
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
// type for function
typedef uint64_t (*hash_function_t)(uint64_t const *, size_t);
#endif  
	
    // Creates hash table and returns its id, parameter is a hash_function
    // used for that hash table.
    unsigned long hash_create(hash_function_t);
    
    // Deletes hash table with given id or reports error if it does not exist.
    void hash_delete(unsigned long id);
    
    // Returns size of hash table with given id or reports error if
    // it does not exist.
    size_t hash_size(unsigned long id);
    
    // Inserts sequence to hash table with given id, reports errors if
    // such hash table does not exist, seq is NULL or size is 0
    // It returns true only if there are no errors and there was no such sequence
    // before
    bool hash_insert(unsigned long id, uint64_t const * seq, size_t size);
    
    // Removes sequence to hash table with given id, reports errors if
    // such hash table does not exist, seq is NULL or size is 0
    // It returns true only if there are no errors and there was such sequence
    // before    
    bool hash_remove(unsigned long id, uint64_t const * seq, size_t size);
    
    // Clears hash table with given id or reports error if it does not exist.
    void hash_clear(unsigned long id);
    
    // Tests if sequence is inside hash table with given id, reports errors if
    // such hash table does not exist, seq is NULL or size is 0
    // It returns true only if there are no errors and sequence is present.
    bool hash_test(unsigned long id, uint64_t const * seq, size_t size);

#ifdef __cplusplus
    }
}
#endif

#endif //HASH_H

