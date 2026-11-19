#include <stddef.h>

void memcpy_nt_avx2(volatile void *__restrict dst,
                    const void *__restrict src,
                    size_t bytes,
                    unsigned int chunk);

void memcpy_nt_avx2_unfenced(volatile void *__restrict dst,
                             const void *__restrict src,
                             size_t bytes,
                             unsigned int chunk);

#define CHUNK_SIZE 16384