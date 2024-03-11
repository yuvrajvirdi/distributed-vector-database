#ifndef FNV_HASH_H
#define FNV_HASH_H

#include <string>

class FNVHash {
public:
    static uint32_t hash(const std::string& input);
};

#endif // FNV_HASH_H