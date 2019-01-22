all: sud

sud: sud.c
	gcc -O sud.c -std=c99 -o sud

test: sud
	./test.sh

clean:
	rm -f sud
