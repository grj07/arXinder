

output: object 
	gcc -o arXinder main.o nav.o menu.o nc_utils.o utils.o -lncursesw -L/usr/lib  -lcrypt -ldl -lm -lpython3.10 -ljansson
	rm *.o
object:	
	gcc -c -g C_source/nav.c C_source/menu.c C_source/nc_utils.c C_source/utils.c -Wno-unused-result -Wsign-compare -march=x86-64 -mtune=generic -O3 -pipe -fno-plt -fexceptions -Wp,-D_FORTIFY_SOURCE=2 -fstack-clash-protection -fcf-protection -flto=auto -ffat-lto-objects -DNDEBUG -g -fwrapv -O3 -Wall
	gcc -c -g C_source/main.c -I/usr/include/python3.10 -I/usr/include/python3.10 -Wno-unused-result -Wsign-compare -march=x86-64 -mtune=generic -O3 -pipe -fno-plt -fexceptions -Wp,-D_FORTIFY_SOURCE=2 -fstack-clash-protection -fcf-protection -flto=auto -ffat-lto-objects -DNDEBUG -g -fwrapv -O3 -Wall
