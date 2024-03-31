#ifndef MURMUR_HASH_3_H
#define MURMUR_HASH_3_H

#include <string>

class MurmurHash3 {
public:
    static uint32_t hash_key(const std::string& key, uint32_t seed, uint32_t ring_size);
};

#endif