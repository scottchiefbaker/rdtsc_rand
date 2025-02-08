default:
	@gcc -O3 -DDEBUG=$(DEBUG) test.c -o rand64

clang:
	@clang -O3 -DDEBUG=$(DEBUG) test.c -o rand64

run: default
	@./rand64 24
