OUT = tpmdos

debug:
	#gcc *.c  -D_GNU_SOURCE -DDEBUG -ltspi -Wall  -g -o $(OUT)
	gcc *.c  -D_GNU_SOURCE -DDEBUG -ltspi -Wunused-function  -g -o $(OUT)
	
all:
	gcc *.c -D_GNU_SOURCE -DNDEBUG -ltspi -Wunused-value -O2 -o $(OUT)
	
install:
	mkdir -p ~/tpmdos
	cp -f tpmdos hourly_send.sh run.sh ~/tpmdos
	
uninstall:
	rm -rf ~/tpmdos
	
clean:
	rm -f $(OUT)