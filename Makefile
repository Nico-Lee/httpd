ROOT_PATH=$(shell pwd)
BIN=httpd
SRC=httpd.c
OBJ=$(SRC:.c=.o)
CC=gcc
LDFLAGS=-lpthread
CGI=$(ROOT_PATH)/htdoc/cgi $(ROOT_PATH)/mysql_connect
.PHONY:all
all:$(BIN) cgi 

$(BIN):$(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS) 
%.o:%.c
	$(CC) -c $< $
.PHONY:cgi
cgi:
	for name in `echo $(CGI)`;do \
		cd $$name;\
		make;\
		cd -;\
	done


.PHONY:clean
clean:
	rm -rf $(BIN) *.o core.*
	for name in `echo $(CGI)`;\
		do\
		cd $$name;\
		make clean;\
		cd -;\
	done
