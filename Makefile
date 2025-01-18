default:
	@g++ test.c -o rand64

run: default
	@./rand64 24
