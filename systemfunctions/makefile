
src=$(wildcard *.c)

target=$(patsubst %.c, %,$(src)) 


all:$(target)

%:%.c
	gcc $< -o $@

clean:
	rm -rf $(target)

.PHONY:clean all "大小写无所谓
