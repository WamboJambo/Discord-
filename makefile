CC = g++
CFLAGS = -Wall -std=c++0x
LIB = -lcurl -ljsoncpp

make:
	$(CC) $(CFLAGS)	test.C -o test $(LIB)
