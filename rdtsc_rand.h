#include <stdint.h>
#include <time.h> // for clock_gettime()

////////////////////////////////////////////////////////////////////////////////
// rdtsc_rand.h: v0.7
//
// https://github.com/scottchiefbaker/rdtsc_rand
////////////////////////////////////////////////////////////////////////////////

// Multiply-Shift Hash (Passes SmallCrush and PractRand up to 128GB)
static uint64_t hash_msh(uint64_t x) {
	uint64_t prime = 0x9e3779b97f4a7c15; // A large prime constant
	x ^= (x >> 30);
	x *= prime;
	x ^= (x >> 27);
	x *= prime;
	x ^= (x >> 31);
	return x;
}

// MurmurHash3 Finalizer (Passes SmallCrush and PractRand up to 32GB)
static uint64_t hash_mur3(uint64_t x) {
	x ^= x >> 33;
	x *= 0xff51afd7ed558ccd;
	x ^= x >> 33;
	x *= 0xc4ceb9fe1a85ec53;
	x ^= x >> 33;
	return x;
}

#if (defined(__ARM_ARCH))
// Nanoseconds since Unix epoch
static uint64_t rdtsc_nanos() {
	struct timespec ts;

	int8_t ok = clock_gettime(CLOCK_MONOTONIC, &ts);  // Uptime
	//int8_t ok = clock_gettime(CLOCK_REALTIME, &ts); // Since epoch

	if (ok != 0) {
		return 0; // Return 0 on failure (you can handle this differently)
	}

	// Calculate nanoseconds
	uint64_t ret = (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec;

	//printf("N: %llu\n", ret);

	return ret;
}
#endif

//////////////////////////////////////
// End hashing function definitions //
//////////////////////////////////////

#if defined(_WIN32) || defined(_WIN64)
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#endif

// CPUID stuff is only for X86 + GCC/Clang, MSVC has it in intrin.h
#if defined(__x86_64) && (defined(__GNUC__) || defined(__clang__))
#include <cpuid.h>
#endif

#if defined(__ARM_ARCH) && __ARM_ARCH >= 8
#define HAS_RANDR 1
#endif

#ifdef __x86_64
#define HAS_RDRAND 1
#endif

// Returns 1 if hardware has RNG, 0 otherwise
int has_hwrng() {
	static int8_t ret = -1;

	if (ret != -1) {
		return ret;
	}

#ifdef HAS_RDRAND
    unsigned int eax, ebx, ecx, edx;
    __get_cpuid(1, &eax, &ebx, &ecx, &edx);
    ret = (ecx & bit_RDRND) != 0;
#elif HAS_RANDR
	uint64_t features;
	asm volatile("mrs %0, ID_AA64ISAR0_EL1" : "=r"(features));
	ret = (int)(((features >> 60) & 0xF) != 0);  // Check RNDR bit field
#else
	ret = 0;
#endif

	return ret;
}

// Returns 1 on success, 0 on failure
int get_hw_rand64(uint64_t* value) {
#ifdef HAS_RDRAND
    unsigned char ok;
    asm volatile("rdrand %0; setc %1" : "=r" (*value), "=qm" (ok) : : "cc");
    return ok ? 1 : 0;
#elif HAS_RANDR
	asm volatile("mrs %0, s3_3_c2_c4_0" : "=r"(*value));
    return 1;
#else
	return 0;
#endif
}

// Get the instruction counter for various CPU/Platforms
static uint64_t get_rdtsc() {
#if defined(_WIN32) || defined(_WIN64)
	return __rdtsc();
#elif defined(__aarch64__) || defined(__arm64)
	uint64_t count;
	__asm__ volatile ("mrs %0, cntvct_el0" : "=r" (count));
	return count;
#elif defined(ARDUINO)
	return micros();
#elif (defined(__x86_64) || defined(__i386__) || defined(__i686__)) && (defined(__GNUC__) || defined(__clang__))
	uint32_t low, high;
	__asm__ volatile ("rdtsc" : "=a"(low), "=d"(high));
	return ((uint64_t)(high) << 32) | low;
#elif (defined(__ARM_ARCH))
	return rdtsc_nanos();
#else
	#warning "rdtsc_rand: Unknown system type. Results will be 0."
	return 0;
#endif
}

// Get an unsigned 64bit random integer
static uint64_t rdtsc_rand64() {
	// Hardware rand supported by x86_64 and ARM 8.5+
	if (has_hwrng()) {
		uint64_t num = 0;
		int8_t ok    = get_hw_rand64(&num);

		// If it's NOT ok we keep go and use rdtsc instead
		if (ok) { return num; }
	}

	// Hash the rdtsc value through hash64
	uint64_t rdtsc_val = get_rdtsc();
	uint64_t ret       = hash_msh(rdtsc_val);

	return ret;
}
