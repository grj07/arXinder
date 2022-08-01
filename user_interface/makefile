filename=ui

output: object 
	gcc -o ${filename} ${filename}.o gfuncs.o -lncurses
	rm *.o
	
object:	
	rsync /home/guy/arXinder/user_interface/gfuncs.* .
	gcc -c -g gfuncs.c -lncurses
	gcc -c -g ${filename}.c -lncurses

