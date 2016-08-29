ROOT_PATH=$(shell pwd)
INCLUDE=-I$(ROOT_PATH)/../lib/include
LIB_PATH=-L$(ROOT_PATH)/../lib/lib/mysql/
LIB=-lmysqlclient

.PHONY:all
all:cgi_insert cgi_select
cgi_insert:cgi_insert.cpp sql_api.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LIB) 
cgi_select:cgi_select.cpp sql_api.cpp
	g++ -o $@ $^ $(INCLUDE) $(LIB_PATH) $(LIB)
	cp cgi_insert $(ROOT_PATH)/../htdoc/cgi/
	cp cgi_select $(ROOT_PATH)/../htdoc/cgi/

.PHONY:clean
clean:
	rm -f cgi_insert cgi_select

#Test:Test.c
#	gcc -o $@ $^ -I./include -L./lib -lmysqlclient
#.PHONY:clean
#clean:
#	rm Test
