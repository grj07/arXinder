/*gfuncs.c - useful functions for arXinder*/
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "gfuncs.h"

#define GRAY 8
#define GRAYR 500
#define GRAYG 500
#define GRAYB 500

#define LRED 9
#define LREDR 1000
#define LREDG 800
#define LREDB 800

#define LYELLOW 10 
#define LYELLOWR 855
#define LYELLOWG 855
#define LYELLOWB 700

void bomb(char *msg)
{
	endwin();
	puts(msg);
	exit(1);
}

void typewriter(char *typed_text, int timestep)
		//Timestep in milliseconds
{
	char *ptr;
	ptr = typed_text;
	while(*ptr)
	{
		addch(*ptr);
		ptr++;
		refresh();
		napms(timestep);
	}
}

//Title screen on start of program
void title_screen(void)
{
	char title1[]="ar";
	char title2[]="inder";
	char subtitle[]="sort and file the new papers from the arXiv.";
	int disp = (strlen(title1)+strlen(title2)+1)/2;
	int kk;

	curs_set(0);
	move(LINES/2,COLS/2-disp);
	attrset(COLOR_PAIR(1));
	typewriter(title1,80);
	refresh();
	attrset(COLOR_PAIR(2)|A_BOLD);
	move(LINES/2-1,COLS/2-disp-1+strlen(title1));
	addch('\\');
	move(LINES/2+1,COLS/2-disp-1 +strlen(title1));
	addch('/');
	move(LINES/2+1,COLS/2-disp+1+strlen(title1));
	addch('\\');
	move(LINES/2-1,COLS/2-disp+1 +strlen(title1));
	addch('/');
	move(LINES/2,COLS/2-disp+strlen(title1));
	addch('X');
	refresh();
	napms(100);
	
	attrset(COLOR_PAIR(1));
	typewriter(title2,50);
	
	napms(500);
	attrset(COLOR_PAIR(3));
	move(LINES/2+2,COLS/2-strlen(subtitle)/2);
	typewriter(subtitle,70);
	move(1,1);
	for(kk=1;kk<=LINES/2-2;kk++)
	{
		deleteln();
		refresh();
		napms(40);
	}
	for(kk=1;kk<=COLS/2-disp-1;kk++)
	{
		delch();
		move(2,1);
		delch();
		move(3,1);
		delch();
		move(4,1);
		delch();
		move(1,1);
		refresh();
		napms(30);
	}
	refresh();
	noecho();
	move(4,strlen(subtitle)/2+strlen(title1)/2+1+strlen(title2)/2);
	delch();
	insch(':');
	refresh();
}

//Bordered window with content
WINDOW *bwinc(char *title, char *content, int height, int width, int ypos,int xpos)
{	
	WINDOW *f_win, *c_win;

	f_win = newwin(height,width,ypos,xpos);
	wbkgd(f_win,COLOR_PAIR(3));
	box(f_win, 0 , 0);		
	mvwprintw(f_win,1,width/2-strlen(title)/2,title);
	c_win = derwin(f_win,height-3,width-4,2,2);
	mvwprintw(c_win,0,0,content);
	wrefresh(f_win);		/* Show that box 		*/

	return f_win;
}

void update_win(WINDOW *win,char *new_content)
{
	WINDOW *c_win;
	int height, width;
	getmaxyx(win,height,width);

	c_win = derwin(win,height-3,width-4,2,2);
	wbkgd(c_win,COLOR_PAIR(4));
	wclear(c_win);
	mvwprintw(c_win,0,0,new_content);
	wrefresh(c_win);
}

void colours()
{
	if(!has_colors())
		bomb("Terminal cannot do colours!");
	else if(start_color()!=OK)
		bomb("Unable to start colours!");
	else if(!can_change_color())
		bomb("Can't change color");
	else
	{
	init_color(LRED,LREDR,LREDG,LREDB);
	init_color(GRAY,GRAYR,GRAYG,GRAYB);
	init_color(LYELLOW,LYELLOWR,LYELLOWG,LYELLOWB);
	init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(2,GRAY,COLOR_BLACK);
	init_pair(3,LRED,COLOR_BLACK);
	init_pair(4,LYELLOW,COLOR_BLACK);
	}	
}

