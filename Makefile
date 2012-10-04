OUT = test

debug:
	gcc *.c  -D_GNU_SOURCE -DDEBUG -ltspi -Wall -g -o $(OUT)
    
all:
	gcc *.c -D_GNU_SOURCE -DNDEBUG -ltspi -Wunused-value -O2 -o $(OUT)
	
clean:
	rm -f $(OUT)