#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rdtsc_rand.h"
#include <inttypes.h>

#define BUF_SIZE 512

// Borrowed from: https://www.pcg-random.org/posts/how-to-test-with-practrand.html
void practrand_output() {
	freopen(NULL, "wb", stdout);  // Only necessary on Windows, but harmless.
	static uint64_t buffer[BUF_SIZE];

	while (1) {
		for (size_t i = 0; i < BUF_SIZE; ++i) {
			buffer[i] = rdtsc_rand64();
		}

		fwrite((void*) buffer, sizeof(buffer[0]), BUF_SIZE, stdout);
	}
}

void human_output(uint64_t iters) {
	printf("RDTSC Counter: %" PRIu64 "\n\n", get_rdtsc());

	for (int i = 0; i < iters; i++) {
		uint64_t num = rdtsc_rand64();

		printf("%" PRIu64 "\n", num);
	}
}

int main(int argc, char *argv[]) {
	// This is the PractRand test output mode
	if (argc > 1 && (strcmp(argv[1], "--pract") == 0)) {
		practrand_output();
	// The default human readable mode
	} else {
		// Default value
		uint64_t iters = 10;

		// If there is a num in ARGV we use that
		if (argc > 1) {
			iters = atoi(argv[1]);
		}

		human_output(iters);
	}
}

// vim: tabstop=4 shiftwidth=4 noexpandtab autoindent softtabstop=4

