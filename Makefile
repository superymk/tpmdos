OUT = test

all:
	gcc tpm.c cpu.c test.c -D_GNU_SOURCE -ltspi -Wall -g -o $(OUT)
    
clean:
	rm -f $(OUT)