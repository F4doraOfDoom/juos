#ifndef KERNEL_BITSET_H
#define KERNEL_BITSET_H

#define CHAR_BIT sizeof(uint32_t)

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a) |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

namespace bitset
{
    inline void set(uint32_t* val, uint32_t idx) {
        *val |= (1 << (idx - 1));
    }

    inline bool test(uint32_t* val, uint32_t idx) {
        return (*val & (1 << (idx - 1)));
    }
}

#endif // KERNEL_BITSET_H