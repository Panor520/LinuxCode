
src=$(wildcard ./src/*.c)
obj=$(patsubst ./src/%.c, ./obj/%.o, $(src))



myDebugArgs= -Wall -g

head_Path=./inc 



all:math.out


math.out:$(obj)
	gcc $^ -o $@ $(myDebugArgs)


$(obj):./obj/%.o:./src/%.c
	gcc -c $< -o $@ $(myDebugArgs) -I $(head_Path)


clean:
	-rm -rf $(obj) math.out 


.PHONY:clean all
