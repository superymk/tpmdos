all:
	gcc tpm.c test.c -ltspi -Wall -g
#all:
	#gcc main.cpp hello.cpp factorial.cpp -o hello
    
clean:
	rm -f *.o