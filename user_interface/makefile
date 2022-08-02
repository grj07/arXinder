filename=ui

output: object 
	gcc -o ${filename} ${filename}.o gfuncs.o -lncursesw
	rm *.o
	
object:	
	rsync /home/guy/arXinder/user_interface/gfuncs.* .
	gcc -c -g gfuncs.c -lncursesw
	gcc -c -g ${filename}.c -lncursesw

