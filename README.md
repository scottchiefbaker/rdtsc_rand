# rdtsc_rand

64bit random numbers with no seeding required

## What does it do?

rdtsc_rand usess the `rdtsc` CPU instruction that is present on all modern
64bit CPUs as a starting point for generating random numbers.
[RDTSC](https://en.wikipedia.org/wiki/Time_Stamp_Counter) is a count of the
_instructions_ that your CPU has run since it booted up. Modern CPUs perform
[billions](https://en.wikipedia.org/wiki/Instructions_per_second#CPU_results)
of instructions per second. Even when idle your CPU is processing a lot of
things in the background so the RDTSC counter is constantly incrementing.

RDTSC is unpredictable. There is not an easy way to know how many
instructions are going on in the background. With billions happening
every second there is no easy way to predict or guess with any accuracy
what that number will be.

## Usability

**rdtsc_rand** was designed from the ground up to be very simple to
implement. It's a single C `.h` header file and is less than 1k in size. To
use **rdtsc_rand** simply include the header file and call the `rdtsc_rand()`
function. See the test.c example file included in this repo for an example.

```
#include <stdint.h>
#include "rdtsc_rand.h"

uint64_t num = rdtsc_rand64();
```

## How fast is it?

Reading the `rdtsc` value and then hashing it to a 64bit integer is extremely
fast and efficient. There are only a handful of instructions required to
read the value, and then a single multiplication instruction to hash it.

## Should I use it for all my random numbers?

No! **rdtsc_rand** is not vetted for true randomness. It is designed to be
a good _starting point_ for other pseudo random number generators. If you
use the numbers from **rdtsc_rand** as seeds for other PRNGs you can have
self-starting good random numbers.

**rdtsc_rand** is not seedable and thus not repeatable. Repeatability can
be helpful for testing and validating code. You should use a seedable PRNG
to get repeatable random numbers. The Minecraft seed is a good example of
randomness that is repeatable.

## Do you recommend any PRNGs to use with rdtsc_rand?

I'm a big fan of [PCG](https://www.pcg-random.org/) and have
used it for my testing. The [xoshiro / xoroshiro](https://prng.di.unimi.it/)
family of PRNGs are also good.
