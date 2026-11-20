#ifndef DOUBLECLIQUE_PAYLOAD_HASH_H
#define DOUBLECLIQUE_PAYLOAD_HASH_H

#include <stdint.h>
#include <string.h>
#include <nmmintrin.h>

// End to end check of key | value | version. Four independent CRC32C lanes so the CPU can overlap them.
static inline uint32_t payload_hash_crc(const char *data, uint32_t len) {
    uint64_t lane0 = len;
    uint64_t lane1 = 0x9e3779b9;
    uint64_t lane2 = 0x85ebca6b;
    uint64_t lane3 = 0xc2b2ae35;
    uint64_t words[4];
    uint64_t word;
    uint32_t half_word;

    if (len >= sizeof(words)) {
        while (len >= sizeof(words)) {
            memcpy(words, data, sizeof(words));
            lane0 = _mm_crc32_u64(lane0, words[0]);
            lane1 = _mm_crc32_u64(lane1, words[1]);
            lane2 = _mm_crc32_u64(lane2, words[2]);
            lane3 = _mm_crc32_u64(lane3, words[3]);
            data += sizeof(words);
            len -= (uint32_t) sizeof(words);
        }

        lane0 = _mm_crc32_u64(lane0, lane1);
        lane0 = _mm_crc32_u64(lane0, lane2);
        lane0 = _mm_crc32_u64(lane0, lane3);
    }

    while (len >= sizeof(word)) {
        memcpy(&word, data, sizeof(word));
        lane0 = _mm_crc32_u64(lane0, word);
        data += sizeof(word);
        len -= (uint32_t) sizeof(word);
    }

    if (len >= sizeof(half_word)) {
        memcpy(&half_word, data, sizeof(half_word));
        lane0 = _mm_crc32_u32((uint32_t) lane0, half_word);
        data += sizeof(half_word);
        len -= (uint32_t) sizeof(half_word);
    }

    while (len > 0) {
        lane0 = _mm_crc32_u8((uint32_t) lane0, (uint8_t) *data);
        data++;
        len--;
    }

    return (uint32_t) lane0;
}

#endif //DOUBLECLIQUE_PAYLOAD_HASH_H
