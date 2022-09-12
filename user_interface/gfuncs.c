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
	else
		;
}

//removes colour if qu true (!=0)
void rem_colour(int ncp,int qu)
{
	if(qu)
		attroff(COLOR_PAIR(ncp));
	else
		;
}

int line_counter(char *filename)
{
	FILE *f = fopen(filename, "r"); /*opening the first subject file*/
	if(f==NULL)
		return(0);
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
int new_scroll_value(WINDOW *win,int tot_lines,int scv)
{
	int  height;
	int scl;
	getmaxyx(win,height,scl);
	scl = min(tot_lines - (height-3),scv);
	scl = max(0,scl);
	return(scl);
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

//Returns number of lines in array. Sets line array to list of strings with whole words
char **to_lines(char *content, int line_width , int *no_of_lines)
{
	int cc=0, lc =1,ccmax =strlen(content);
	int *nl_pos= malloc(sizeof(int)*ccmax); /*an array the size of the content isn't going to be too small*/
	int prev_newline_pos = 0;
	int prev_space_pos = 0;
	nl_pos[0]=-1;
	for(cc = 0; cc < ccmax; cc++) /*populates array with (ought-to-be) new-line char positions*/
	{
		if((char) content[cc]==' ')	
		{
			if(cc-prev_newline_pos>=line_width)
			{
				nl_pos[lc] = prev_space_pos;
				prev_newline_pos = nl_pos[lc];
				lc++;
			}
			prev_space_pos = cc;
		}
	}	
	if(ccmax-prev_newline_pos>=line_width)
	{
		nl_pos[lc] = prev_space_pos;
		lc++;
	}
	//Now lc gives number of lines, nl_pos[i<lc] gives end of line positions 
	*no_of_lines = lc;
	nl_pos[*no_of_lines]=ccmax;
	//move(1,5);
	//printw("The new line goes at %d",nl_pos[1]);
	//refresh();

	//to copy lines to array
	size_t line_size = (line_width+1)*sizeof(char);
	char *line_array_chs = calloc((*no_of_lines)*(line_width+1),sizeof(char));
	char **line_array_lines = malloc(sizeof(char *)*(*no_of_lines));
	for(lc = 0;lc<*no_of_lines;lc++)
	{
		for(cc = 0;cc<nl_pos[lc+1]-nl_pos[lc]-1;cc++) /*copies first line*/
		{
			*(line_array_chs+cc+lc*line_size) = content[cc+nl_pos[lc]+1];
		}
		line_array_chs[lc*line_size+nl_pos[lc+1]-nl_pos[lc]]='\0';
		*(line_array_lines+lc) = line_array_chs+lc*line_size;
	}
	free(nl_pos);
	return(line_array_lines);
}

void free_lines(char **lines_buf)
{
	free(*lines_buf);
	free(lines_buf);
}

