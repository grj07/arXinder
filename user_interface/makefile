filename=ui

output: object 
	gcc -o ${filename} ${filename}.o gfuncs.o pfuncs.o -lncurses
	rm *.o
	
object:	
	gcc -c -g gfuncs.c pfuncs.c -lncurses
	gcc -c -g ${filename}.c -lncurses

