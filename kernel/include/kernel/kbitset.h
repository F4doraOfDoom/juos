#ifndef KERNEL_BITSET_H
#define KERNEL_BITSET_H

#define CHAR_BIT sizeof(uint32_t)

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a) |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

// bitset operations
namespace bitset
{
    /**
     * @brief Set bit _idx_ in _val_
     * 
     * @param val 
     * @param idx 
     */
    inline void set(uint32_t* val, uint32_t idx) {
        *val |= (1 << (idx - 1));
    }

    /**
     * @brief check if bit _idx_ in _val_ is 1
     * 
     * @param val 
     * @param idx 
     * @return true 
     * @return false 
     */
    inline bool test(uint32_t* val, uint32_t idx) {
        return (*val & (1 << (idx - 1)));
    }
}

#endif // KERNEL_BITSET_H