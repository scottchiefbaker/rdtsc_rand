#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// rdtsc_rand.h: v0.2
//
// https://github.com/scottchiefbaker/rdtsc_rand
////////////////////////////////////////////////////////////////////////////////

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

// Borrowed and (slightly modified) from
// https://elixir.bootlin.com/linux/v6.11.5/source/include/linux/hash.h
uint64_t hash64(uint64_t val) {
	return (val * 0x61c8864680b583ebull);
}

// Get an unsigned 64bit random integer
uint64_t rdtsc_rand64() {
	// Hash the rdtsc value through hash64
	uint64_t rdtsc_val = get_rdtsc();
	uint64_t ret       = hash64(rdtsc_val);

	return ret;
}
