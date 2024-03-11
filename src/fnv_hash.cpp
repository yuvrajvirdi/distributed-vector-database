#include "fnv_hash.h"

/**
 * @brief 
 * 
 * @param input 
 * @return uint32_t 
 */
uint32_t FNVHash::hash(const std::string& input) {
    const uint32_t fnv_prime = 16777619;
    const uint32_t offset_basis = 2166136261;
    uint32_t hash = offset_basis;

    for (char c : input) {
        hash ^= static_cast<uint32_t>(c);
        hash *= fnv_prime;
    }

    return hash;
}