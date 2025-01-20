default:
	@gcc test.c -o rand64

clang:
	@clang test.c -o rand64

run: default
	@./rand64 24
