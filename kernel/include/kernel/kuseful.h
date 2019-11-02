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

#define IN_RANGE_X(n, min, max) ((n < min) || (n > max) ? 1 : 0)
#define IN_RANGE_C(n, min, max) ((n <= min) || (n >= max) ? 1 : 0)

// bitset
#define BIT_MASK(b)             (1 << ((b)))
#define BIT_SET(a, b, off)      ((a)[b] |= BIT_MASK(off))
#define BIT_CLEAR(a, b, off)    ((a)[b] &= ~BIT_MASK(off))
#define BIT_TEST(a, b, off)     ((a)[b] & BIT_MASK(off))


//#define BIT_NSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

#endif //K_USEFUL_H_