#include <iostream>
#include <string.h>
#include <assert.h>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <vector>
#include "hash.h"

namespace {

#ifndef NDEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif

using namespace std;

struct Hash
{
    hash_function_t hash_function;

    Hash(hash_function_t function) : hash_function(function) {}

    uint64_t operator()(vector <uint64_t> const& v) const
    {
        uint64_t hash_value = hash_function(&v[0], v.size());
        return hash_value;
    }
};

// Types used to store sequence
using sequence_64_t = vector <uint64_t>;

// Unordered set storing sequences with the same hash.
using hash_set = unordered_set <sequence_64_t, Hash>;

// Unordered_map for all hash_sets, the key is 'id' of
// the hash_set, the value is the hash_set
using set_map = unordered_map < unsigned long, hash_set>;

// Those two functions prevent static initialization order fiasco
set_map& hash_tables() {
    static set_map hash_tables;
    return hash_tables;
} 

unsigned long tables_counter() {
    static unsigned long tables_counter = 0;
    tables_counter++;
    return tables_counter;
}

// helper function converting pointer to vector
sequence_64_t get_sequence(uint64_t const * seq, size_t size) {
    sequence_64_t sequence;
    for (size_t i = 0 ; i < size; i++) {
        sequence.push_back(*seq);
        seq++;
    }
    return sequence;
}

// function converting vector to string
// with proper spaces
string string_sequence(sequence_64_t & sequence) {
    size_t size = sequence.size();
    string result = "";
    if (size == 0)
        return result;
    for (size_t i = 0; i < size - 1; i++) {
        result += to_string(sequence[i]);
        result += " ";
    }
    result += to_string(sequence[size - 1]);
    return result;
}

// debug function used to print message
void debug_log(const string & message) {
    if (!debug)
        return;
    static ios_base::Init init;
    cerr << message << "\n";
}

// debug function used only in hash_create, 
// used to print hash_function
void debug_create(hash_function_t hash_function) {
    if (!debug)
        return;
    static ios_base::Init init;
    cerr << "hash_create(" << &hash_function << ")\n"; 
}

// log if function is called only with id
void log_init(const string & function, unsigned long id) {
    if (!debug) 
        return;
    string message = function;
    message += "(";
    message += to_string(id); 
    message += ")";
    debug_log(message);
}

// log if function is called with id and sequence
void log_init_seq(const string & function, unsigned long id, 
                  const uint64_t * seq, size_t size) {
    if (!debug)
        return;                  
    string message = function;
    message += "(";
    message += to_string(id);
    message += ", ";
    if (seq == NULL)
        message += "NULL";
    else if (size == 0) {
        message += "\"\"";
    } else {
        message += "\"";
        for (size_t i = 0; i < size-1; i++) {
            message += to_string(*seq);
            message += " ";
            seq++;
        }
        message += to_string(*seq);
        message += "\"";
    }
    message += ", ";
    message += to_string(size);
    message += ")";
    debug_log(message);
}    

// log when there is message after function name and id
void log_action(const string & function, unsigned long id,
                const string & action_message) {
    if (!debug)
        return;
    string message = function;
    message += ": hash table #";
    message += to_string(id);
    message += action_message;
    debug_log(message);
}

// function used to validate arguments passed to function
bool validate_arguments(const string & function, 
                        uint64_t const * seq, size_t size) {
    bool is_correct = true;
    if (seq == NULL) {
        string message = function;
        message += ": invalid pointer (NULL)";
        is_correct = false;
        debug_log(message);
    }
    if (size == 0) {
        string message = function;
        message += ": invalid size (0)";
        is_correct = false;
        debug_log(message);
    }
    return is_correct;
}

void log_table_not_exist(const string & function, unsigned long id) {
    if (!debug) 
        return;
    string message = function;
    message += ": hash table #";
    message += to_string(id);
    message += " does not exist";
    debug_log(message);
}

// function which creates log message for sequence insert/delete/test
void log_sequence(const string & function, sequence_64_t & sequence, 
                  unsigned long id, const string & action_message) {
        if (!debug)
            return;
        string message = function;
        message += ": hash table #";
        message += to_string(id);
        message += ", sequence \"";
        message += string_sequence(sequence);
        message += "\" ";
        message += action_message;
        debug_log(message);
}

} /* anonymous namespace */

namespace jnp1 {

unsigned long hash_create(hash_function_t hash_function) {
    debug_create(hash_function);
    unsigned long current_id = tables_counter();
    hash_tables().emplace(current_id, hash_set(0, {hash_function}));
    log_action((string) __func__, current_id, " created");
    return current_id;
}


void hash_delete(unsigned long id) {
    log_init((string) __func__, id);
    auto it = hash_tables().find(id);
    if (it == hash_tables().end()) {
        log_table_not_exist(__func__, id);
        return;
    }
    log_action((string) __func__, id, " deleted");
    hash_tables().erase(it);
}


size_t hash_size(unsigned long id) {
    log_init((string) __func__, id);
    auto it = hash_tables().find(id);
    if (it == hash_tables().end()) {
        log_table_not_exist(__func__, id);
        return 0;
    }
    size_t size = it->second.size();
    string action_message = " contains " + to_string(size) + " element(s)";
    log_action((string) __func__, id, action_message);
    return size;
}


bool hash_insert(unsigned long id, uint64_t const * seq, size_t size) {
    log_init_seq((string) __func__, id, seq, size);
    if (!validate_arguments((string) __func__, seq, size)) {
        return false;
    }
    auto it = hash_tables().find(id);
    if (it == hash_tables().end() ) {
        log_table_not_exist(__func__, id);
        return false;
    }
    hash_set * this_hash_set = &(it->second);
    sequence_64_t sequence = get_sequence(seq, size);

    if (this_hash_set->find(sequence) != this_hash_set->end()) {
        log_sequence((string) __func__, sequence, id, "was present");
        return false;
    }
    this_hash_set->insert(sequence);
    log_sequence((string) __func__, sequence, id, "inserted");
    return true;
}


bool hash_remove(unsigned long id, uint64_t const * seq, size_t size) {
    log_init_seq((string) __func__, id, seq, size);
    if (!validate_arguments((string) __func__, seq, size)) {
        return false;
    }
    auto it = hash_tables().find(id);
    if (it == hash_tables().end() ) {
        log_table_not_exist(__func__, id);
        return false;
    }
    hash_set * this_hash_set = &(it->second);
    sequence_64_t sequence = get_sequence(seq, size);

    if (this_hash_set->find(sequence) == this_hash_set->end()) {
        log_sequence((string) __func__, sequence, id, "was not present");
        return false;
    }
    this_hash_set->erase(sequence);
    log_sequence((string) __func__, sequence, id, "removed");
    return true;
}

void hash_clear(unsigned long id) {
    log_init((string) __func__, id);
    auto it = hash_tables().find(id);
    if (it == hash_tables().end()) {
        log_table_not_exist(__func__, id);
        return;
    }

    if (it->second.empty()) {
        log_action((string) __func__, id, " was empty");
    } else {
        it->second.clear();
        log_action((string) __func__, id, " cleared");
    }
}


bool hash_test(unsigned long id, uint64_t const * seq, size_t size) {
    log_init_seq((string) __func__, id, seq, size);
    if (!validate_arguments((string) __func__, seq, size)) {
        return false;
    }
    auto it = hash_tables().find(id);
    if (it == hash_tables().end() ) {
        log_table_not_exist(__func__, id);
        return false;
    }
    hash_set * this_hash_set = &(it->second);
    sequence_64_t sequence = get_sequence(seq, size);

    if (this_hash_set->find(sequence) == this_hash_set->end()) {
        log_sequence((string) __func__, sequence, id, "is not present");
        return false;
    }
    log_sequence((string) __func__, sequence, id, "is present");
    return true;
}

} /* namespace jnp1 */
