/**
 * @file kuseful.h
 * @author Jonathan Uklisty (F4doraOfDoom) (yoniu7@gmail.com)
 * @brief Header file containing useful, common operations
 * @version 0.1
 * @date 01-11-2019
 * 
 * @copyright Copyright Jonathan Uklisty (c) 2019 (MIT License)
 * 
 */

#ifndef K_USEFUL_H_
#define K_USEFUL_H_

inline void* PTR_ADVANCE(void* ptr, uint32_t n) {
    return (void*)((uint8_t*)ptr + n);
}

inline void* PTR_ADD(void* a, void* b) {
    return (void*)((uintptr_t)a + (uintptr_t)b);
}

inline void* PTR_SUB(void* a, void* b) {
    return (void*)((uintptr_t)a - (uintptr_t)b);
}

inline uint32_t GET_BIT_OF(uint32_t value)
{
    return ((2 << (value - 1)) == 0);
}

#define BIT_IS_CLEAR(n, k) (((((uint64_t)n) & (1 << (((uint64_t)k) - 1)))) != 0)

#define BIT_IS_SET(n,k) ((NOT BIT_IS_CLEAR(n, k)))

inline uint64_t ALIGN_VAL(uint64_t value, uint64_t alignment) {
    return value + (-value & (alignment - 1));
}

#define IN_RANGE_X(n, min, max) ((n < min) && (n < max))
#define IN_RANGE_C(n, min, max) ((n >= min) && (n <= max))

// bitset
#define BIT_MASK(b)             (1 << ((b)))
#define BIT_SET(a, b, off)      ((a)[b] |= BIT_MASK(off))
#define BIT_CLEAR(a, b, off)    ((a)[b] &= ~BIT_MASK(off))
#define BIT_TEST(a, b, off)     ((a)[b] & BIT_MASK(off))

// for easy access to bits




//#define BIT_NSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

#endif //K_USEFUL_H_