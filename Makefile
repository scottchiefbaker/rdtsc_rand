DEBUG ?= 0

default:
	@gcc -D DEBUG=$(DEBUG) test.c -o rand64

clang:
	@clang test.c -o rand64

run: default
	@./rand64 24
