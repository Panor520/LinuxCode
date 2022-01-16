
src=$(wildcard *.c)

target=$(patsubst %.c, %,$(src)) 

myArgs=-g -Wall

all:$(target)

%:%.c
	gcc $< -o $@ $(myArgs)

clean:
	rm -rf $(target)

.PHONY:clean all "大小写无所谓
