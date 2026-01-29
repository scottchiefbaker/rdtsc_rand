#include <stdint.h>
#include <time.h> // for clock_gettime()

////////////////////////////////////////////////////////////////////////////////
// rdtsc_rand.h: v0.8
//
// https://github.com/scottchiefbaker/rdtsc_rand
////////////////////////////////////////////////////////////////////////////////

// Variant of SmartMix64 (Passes PractRand to at least 512GB)
uint64_t splitmix64_hash(uint64_t x) {
	x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
	x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
	x =  x ^ (x >> 31);
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
#include <x86intrin.h>
#endif

#if defined(__ARM_ARCH) && __ARM_ARCH >= 8
#define HAS_RNDR 1
#endif

#ifdef __x86_64
#define HAS_RDRAND 1
#endif

// Returns 1 if hardware has RNG, 0 otherwise
int has_hwrng() {
	int8_t ret = 0;

#ifdef HAS_RDRAND
	unsigned int eax = 0, ebx = 0, ecx = 0, edx = 0;
	__get_cpuid(1, &eax, &ebx, &ecx, &edx);
	ret = (ecx & bit_RDRND) != 0;
#elif HAS_RNDR
	uint64_t features;
	__asm__ volatile("mrs %0, ID_AA64ISAR0_EL1" : "=r"(features));
	ret = (int)(((features >> 60) & 0xF) != 0);  // Check RNDR bit field
#endif

	return ret;
}

// Returns 1 on success, 0 on failure
int get_hw_rand64(uint64_t* value) {
#ifdef HAS_RDRAND
	unsigned char ok;
	__asm__ volatile("rdrand %0; setc %1" : "=r" (*value), "=qm" (ok) : : "cc");
	return ok ? 1 : 0;
#elif HAS_RNDR
	__asm__ volatile("mrs %0, s3_3_c2_c4_0" : "=r"(*value));
	return 1;
#else
	*value = 0;
	return 0;
#endif
}

// Get the instruction counter for various CPU/Platforms
static uint64_t get_rdtsc() {
#if defined(_WIN32) || defined(_WIN64)
	return __rdtsc();
#elif (defined(__x86_64) || defined(__i386__) || defined(__i686__)) && (defined(__GNUC__) || defined(__clang__))
	return __rdtsc();
#elif defined(__aarch64__) || defined(__arm64)
	uint64_t count;
	__asm__ volatile ("mrs %0, cntvct_el0" : "=r" (count));
	return count;
#elif defined(ARDUINO)
	return micros();
#elif (defined(__ARM_ARCH))
	return rdtsc_nanos();
#else
	#warning "rdtsc_rand: Unknown system type. Results will be 0."
	return 0;
#endif
}

// Get an unsigned 64bit random integer
static uint64_t rdtsc_rand64() {

#if USE_HWRNG
	// Cache/memoize the results for speed (only check once)
	static int8_t has_hwr = -1;
	if (has_hwr == -1) {
		has_hwr = has_hwrng();
	}

	// Hardware rand supported by x86_64 and ARM 8.5+
	if (has_hwr) {
		uint64_t num = 0;
		// Returns 0/1 if the hwrng is good, if it's not good
		// we fallback to rdtsc below
		if (get_hw_rand64(&num)) {
			return num;
		}
	}
#endif

	// Hash the rdtsc value to get a really random value
	uint64_t rdtsc_val = get_rdtsc();
	uint64_t ret       = splitmix64_hash(rdtsc_val);

	return ret;
}
