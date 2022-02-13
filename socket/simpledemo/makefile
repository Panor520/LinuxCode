
src = $(wildcard *.c)

target = $(patsubst %.c, % ,$(src))


myArgs= -g -Wall -pthread


all:$(target)

%:%.c
	gcc $^ -o $@ $(myArgs)

clean : 
	rm -rf $(target)

.PHONY:clean all
