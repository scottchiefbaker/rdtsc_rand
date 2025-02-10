CC        ?= gcc
DEBUG     ?= 0
USE_HWRNG ?= 0

CFLAGS := -Wall -Wextra -O3

# Add -DUSE_HWRNG=1 to CFLAGS if USE_HWRNG is set to 1
ifneq ($(USE_HWRNG),0)
    CFLAGS += -DUSE_HWRNG=1
endif

# Add debug flags if DEBUG=1
ifneq ($(DEBUG),0)
    CFLAGS += -g -DDEBUG=1
endif

##################################################################

default:
	@$(CC) $(CFLAGS) test.c -o rand64

run: default
	@./rand64 24
