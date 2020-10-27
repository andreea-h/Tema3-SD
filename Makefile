build:
	gcc -Wall -std=c99 tema3.c -o movies
	gcc -Wall -std=c99 pb2.c -o labirint
clean:
	rm movies labirint
