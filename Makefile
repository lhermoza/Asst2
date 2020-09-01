all: fileCompressor.h fileCompressor.o 
	gcc -g -o Asst2 fileCompressor.c
 
fileCompressor: fileCompressor.o fileCompressor.h
	gcc -g -c fileCompressor.c
 
clean:
	rm -f *.o Asst2
