filename=ui

output: object 
	gcc -o arXinder ${filename}.o gfuncs.o pfuncs.o -lncursesw -L/usr/lib  -lcrypt -ldl -lm -lpython3.10 -ljansson
	rm *.o
	
object:	
	gcc -c -g user_interface/gfuncs.c user_interface/pfuncs.c -lncursesw  -Wno-unused-result -Wsign-compare -march=x86-64 -mtune=generic -O3 -pipe -fno-plt -fexceptions         -Wp,-D_FORTIFY_SOURCE=2 -fstack-clash-protection -fcf-protection -flto=auto -ffat-lto-objects -DNDEBUG -g -fwrapv -O3 -Wall
	gcc -c -g user_interface/${filename}.c -lncursesw -I/usr/include/python3.10 -I/usr/include/python3.10 
