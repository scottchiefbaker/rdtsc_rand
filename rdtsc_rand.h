#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// rdtsc_rand.h: v0.3
//
// https://github.com/scottchiefbaker/rdtsc_rand
////////////////////////////////////////////////////////////////////////////////

// Borrowed and (slightly modified) from
// https://elixir.bootlin.com/linux/v6.11.5/source/include/linux/hash.h
// Does not pass SmallCrush
uint64_t hash64(uint64_t val) {
	return (val * 0x61c8864680b583ebull);
}

// Multiply-Shift Hash (Passes SmallCrush)
uint64_t hash_msh(uint64_t x) {
    uint64_t prime = 0x9e3779b97f4a7c15; // A large prime constant
    x ^= (x >> 30);
    x *= prime;
    x ^= (x >> 27);
    x *= prime;
    x ^= (x >> 31);
    return x;
}

// MurmurHash3 Finalizer (Passes SmallCrush)
uint64_t hash_mur3(uint64_t x) {
    x ^= x >> 33;
    x *= 0xff51afd7ed558ccd;
    x ^= x >> 33;
    x *= 0xc4ceb9fe1a85ec53;
    x ^= x >> 33;
    return x;
}

// FNV-1a / Fowler–Noll–Vo hash (Does not pass SmallCrush)
uint64_t hash_fnv(uint64_t x) {
    uint64_t prime = 1099511628211;           // 64-bit FNV prime
    uint64_t hash  = 14695981039346656037ULL; // 64-bit FNV offset basis

    for (int i = 0; i < 8; i++) {
        hash ^= (x & 0xff);
        hash *= prime;
        x >>= 8;
    }
    return hash;
}

//////////////////////////////////////
// End hashing function definitions //
//////////////////////////////////////

#if defined(_WIN32) || defined(_WIN64)
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#endif

// Get the instruction counter for various CPU/Platforms
uint64_t get_rdtsc() {
#if defined(_WIN32) || defined(_WIN64)
	return __rdtsc();
#elif defined(__aarch64__)
	uint64_t count;
	__asm__ volatile ("mrs %0, cntvct_el0" : "=r" (count));
	return count;
#elif defined(ARDUINO)
	return micros();
#elif defined(__GNUC__) || defined(__clang__)
	uint32_t low, high;
	__asm__ volatile ("rdtsc" : "=a"(low), "=d"(high));
	return ((uint64_t)(high) << 32) | low;
#else
	#error "Unsupported platform"
#endif
}

// Get an unsigned 64bit random integer
uint64_t rdtsc_rand64() {
	// Hash the rdtsc value through hash64
	uint64_t rdtsc_val = get_rdtsc();
	uint64_t ret       = hash_msh(rdtsc_val);

	return ret;
}
