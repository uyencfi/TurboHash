#pragma once
#include <cstdint>
#include <atomic>
#include <immintrin.h>
// http://www.cs.utexas.edu/~pingali/CS378/2015sp/lectures/Spinlocks%20and%20Read-Write%20Locks.htm
/* Compile read-write barrier */
#define barrier() asm volatile("": : :"memory")
/* Pause instruction to prevent excess processor bus usage */ 
#define cpu_relax() asm volatile("pause\n": : :"memory")

/** https://stackoverflow.com/questions/30467638/cheapest-least-intrusive-way-to-atomically-update-a-bit
 * \brief Atomically tests and sets a bit (INTEL only)
 * \details Sets bit \p bit of *\p ptr and returns its previous value.
 * The function is atomic and acts as a read-write memory barrier.
 * \param[in] ptr a pointer to an unsigned integer
 * \param[in] bit index of the bit to set in *\p ptr
 * \return the previous value of bit \p bit
 */
inline char turbo_atomic_bittestandset_x86(volatile unsigned int* ptr, unsigned int bit) {
    char out;
#if defined(__x86_64)
    __asm__ __volatile__ (
        "lock; bts %2,%1\n"  // set carry flag if bit %2 (bit) of %1 (ptr) is set
                             //   then set bit %2 of %1
        "sbb %0,%0\n"        // set %0 (out) if carry flag is set
        : "=r" (out), "=m" (*ptr)
        : "Ir" (bit)
        : "memory"
    );
#else
    __asm__ __volatile__ (
        "lock; bts %2,%1\n"  // set carry flag if bit %2 (bit) of %1 (ptr) is set
                             //   then set bit %2 of %1
        "sbb %0,%0\n"        // set %0 (out) if carry flag is set
        : "=q" (out), "=m" (*ptr)
        : "Ir" (bit)
        : "memory"
    );
#endif
    return out;
}
/**
 * \brief Atomically tests and resets a bit (INTEL only)
 * \details Resets bit \p bit of *\p ptr and returns its previous value.
 * The function is atomic and acts as a read-write memory barrier
 * \param[in] ptr a pointer to an unsigned integer
 * \param[in] bit index of the bit to reset in \p ptr
 * \return the previous value of bit \p bit
 */
inline char turbo_atomic_bittestandreset_x86(volatile unsigned int* ptr, unsigned int bit) {
    char out;
#if defined(__x86_64)
    __asm__ __volatile__ (
        "lock; btr %2,%1\n"  // set carry flag if bit %2 (bit) of %1 (ptr) is set
                             //   then reset bit %2 of %1
        "sbb %0,%0\n"        // set %0 (out) if carry flag is set
        : "=r" (out), "=m" (*ptr)
        : "Ir" (bit)
        : "memory"
    );
#else
    __asm__ __volatile__ (
        "lock; btr %2,%1\n"  // set carry flag if bit %2 (bit) of %1 (ptr) is set
                             //   then reset bit %2 of %1
        "sbb %0,%0\n"        // set %0 (out) if carry flag is set
        : "=q" (out), "=m" (*ptr)
        : "Ir" (bit)
        : "memory"
    );
#endif
    return out;
}

#define SPINLOCK_FREE 0
typedef unsigned int turbo_bitspinlock;

inline bool turbo_lockbusy(turbo_bitspinlock *lock, int bit_pos) {
    return (*lock) & (1 << bit_pos);
}

inline void turbo_bit_spin_lock(turbo_bitspinlock *lock, int bit_pos)
{
    
    while(1) {
        // test & set return 0 if success
        if (turbo_atomic_bittestandset_x86(lock, bit_pos) == SPINLOCK_FREE) {
            return;
        }
        while ((*lock) & (1 << bit_pos)) /* cpu_relax();*/ _mm_pause();
    }
    
}

inline void turbo_bit_spin_unlock(turbo_bitspinlock *lock, int bit_pos)
{
    // barrier();
    std::atomic_thread_fence(std::memory_order_acq_rel);
    *lock &= ~(1 << bit_pos);
}

class SpinLockScope {
public:
    static const int kBitLockPosition = 0;
    SpinLockScope(turbo_bitspinlock *lock):
        lock_(lock) {
        turbo_bit_spin_lock(lock, kBitLockPosition);
    }
    ~SpinLockScope() {
        // release the bit lock
        turbo_bit_spin_unlock(lock_, kBitLockPosition);
    }
    turbo_bitspinlock *lock_;
};