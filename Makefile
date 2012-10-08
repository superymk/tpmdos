OUT = tpmdos

debug:
	gcc *.c  -D_GNU_SOURCE -DDEBUG -ltspi -ltpm_unseal -Wall -g -o $(OUT)
	
all:
	gcc *.c -D_GNU_SOURCE -DNDEBUG -ltspi -ltpm_unseal -Wno-unused-value -Wno-unused-result -O2 -o $(OUT)
	
install:
	make all
	mkdir -p ~/tpmdos
	cp -f tpmdos run.sh ~/tpmdos
	
uninstall:
	rm -rf ~/tpmdos
	
clean:
	rm -f $(OUT)