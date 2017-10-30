LIBS = -Iinclude/

all: server client test/test_user

user.o: include/user.h include/user.c
	gcc -c include/user.c -o $@

server: server.c user.o
	gcc $(LIBS) user.o server.c -o $@ -g

client: client.c
	gcc client.c -o $@ -g

test/test_user: test/test_user.c user.o
	gcc $(LIBS) user.o test/test_user.c -o test/test_user -g

check: test/test_user
	test/test_user

clean:
	-rm client server test/test_user
