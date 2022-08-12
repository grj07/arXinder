/*gfuncs2.c - useful general functions for arXinder*/
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "gfuncs.h"

void bomb(char *msg)
	//Quits program and prints message
{
	endwin();
	puts(msg);
	exit(1);
}

//Max and min of two numbers
int max(int a,int b)
{
	if(a>b)
		return(a);
	else
		return(b);
}

int min(int a,int b)
{
	if(a<b)
		return(a);
	else
		return(b);
}
//Types out from cursor position with gap timestep in milliseconds
void typewriter(char *typed_text, int timestep)
{
	char *pos;
	pos = typed_text;
	while(*pos)
	{
		addch(*pos);
		pos++;
		refresh();
		usleep(timestep*1000);
	}
}

//sets colour if qu true (!=0)
void add_colour(int ncp,int qu)
{
	if(qu)
		attron(COLOR_PAIR(ncp));
}

//removes colour if qu true (!=0)
void rem_colour(int ncp,int qu)
{
	if(qu)
		attroff(COLOR_PAIR(ncp));
}

int line_counter(char *filename)
{
	FILE *f = fopen(filename, "r"); /*opening the first subject file*/
	int total_lines=0;
	char c;
	for (c = getc(f); c != EOF; c = getc(f))
        	if (c == '\n') 
            		total_lines++;
	fclose(f);
	return(total_lines);
}


//erases from cursor position
void clear_text(int length)
{
	int kk=0;
	for(kk=0;kk<=length;kk++)
		addch(' ');
}

//New scroll value stops scrolling past top or bottom of text
int new_scroll_value(WINDOW *win,char *content,int scv)
{
	int  height,width;
	int scl;
	getmaxyx(win,height,width);
	scl = max(0,scv);
	scl = min(strlen(content)/(width-4)-(height-4),scl);
	return((int)scl);
}

//Returns the concatenated strings string_1 string_2
char *join_strings(char *string_1, char *string_2)
{
	char *string;
	int stsize, s1size;

	s1size = strlen(string_1);
	stsize = s1size+strlen(string_2);
	string = (char *)malloc((stsize+1)*sizeof(char));
	int i;
	//assigning string_1 to first part of string
	for(i=0;i<s1size;i++)
	{
		*(string + i)= *(string_1+i);
	}
	//assigning string_2 to second part of string
	for(i=s1size;i<stsize;i++)
	{
		*(string + i)= *(string_2+i-s1size);
	}
	*(string+stsize)='\0';/*capping string with end character*/
	return(string);
}
