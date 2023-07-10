output: object 
	gcc -o arXinder main.o nav.o menu.o nc_utils.o utils.o -lncursesw -L/usr/lib  -lcrypt -ldl -lm -lpython3.11 -ljansson
	rm *.o
object:	
	gcc -c -g src/utils.c 
	gcc -c -g src/nav.c src/menu.c src/nc_utils.c -Wno-unused-result -Wsign-compare -march=x86-64 -mtune=generic -O3 -pipe -fno-plt -fexceptions -Wp,-D_FORTIFY_SOURCE=2 -fstack-clash-protection -fcf-protection -flto=auto -ffat-lto-objects -DNDEBUG -g -fwrapv -O3 -Wall
	gcc -c -g src/main.c -I/usr/include/python3.11 -I/usr/include/python3.11 -Wno-unused-result -Wsign-compare -march=x86-64 -mtune=generic -O3 -pipe -fno-plt -fexceptions -Wp,-D_FORTIFY_SOURCE=2 -fstack-clash-protection -fcf-protection -flto=auto -ffat-lto-objects -DNDEBUG -g -fwrapv -O3 -Wall
