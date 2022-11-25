#GCC=gcc -O3 -g -Wall -Wextra -pedantic -std=gnu11
GCC=gcc -g -Wall -Wextra -pedantic -std=gnu11
LD_FLAGS= -lpthread

all: networking

rebuild: clean all

csapp.o: csapp.c csapp.h
	$(GCC) -c $< -o $@

common.o: common.c common.h
	$(GCC) -c $< -o $@

networking: networking.c networking.h common.o csapp.o sha256.o
	$(GCC) $< *.o -o $@ $(LD_FLAGS)

sha256.o : sha256.c sha256.h
	$(CC) $(CFLAGS) -c $< -o $@

zip: ../src.zip

../src.zip: clean
	cd .. && zip -r src.zip src/Makefile src/*.c src/*.h

clean:
	rm -rf *.o networking sha256 vgcore*
