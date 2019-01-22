all: sud

sud: sud.c
	gcc -O sud.c -o sud

test: sud
	./test.sh

clean:
	rm -f sud
