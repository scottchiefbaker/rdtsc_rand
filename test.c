#include <stdio.h>
#include <stdlib.h>
#include "rdtsc_rand.h"
#include <inttypes.h>

int main(int argc, char *argv[]) {
	// Default value
	uint64_t iters = 10;

	// If there is a num in ARGV we use that
	if (argc > 1) {
		iters = atoi(argv[1]);
	}

	for (int i = 0; i < iters; i++) {
		uint64_t num = rdtsc_rand64();

		printf("%" PRIu64 "\n", num);
	}
}

// vim: tabstop=4 shiftwidth=4 noexpandtab autoindent softtabstop=4

