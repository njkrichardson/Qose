CC = gcc 
CFLAGS = -Wall -O3 -I./include -I./hash-sigs -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib

demo: src/arm_demo.c hash-sigs/hss_lib_thread.a
	$(CC) $(CFLAGS) src/arm_demo.c hash-sigs/hss_lib_thread.a -lcrypto -lpthread -o demo
