run: main.c
	clang -Wall -Wextra main.c -o main -lncurses
	./main
	rm main

build: main.c
	clang -Wall -Wextra main.c -o main -lncurses

clean: main
	rm -f main