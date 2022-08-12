/*pfuncs.c - particular functions for arXinder*/
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <Python.h>
#include "pfuncs.h"
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


//This function runs Python. Threaded with the title screen and navigator
void *call_datprc()
{
	char filename[] = "datprc.py";
	FILE* fp;

	Py_Initialize();

	fp = fopen(filename, "r");
	PyRun_SimpleFile(fp, filename);

	Py_Finalize();
	return NULL;
}

int colours() /*Colours checks if the terminal can do colours, and returns 1 if so*/
{
	int qu=0;
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
	init_pair(5,COLOR_RED,LYELLOW);
	qu = 1;
	}	
	return(qu);
}

void *title_screen(int col)
{
	char title1[]="ar";
	char title2[]="inder";
	char subtitle[]="sort and file the new papers from the arXiv.";
	int disp = (strlen(title1)+strlen(title2)+1)/2;
	int kk;

	curs_set(0);
	move(LINES/2,COLS/2-disp);
	add_colour(1,col);
	attrset(COLOR_PAIR(1));
	typewriter(title1,120);
	refresh();
	add_colour(2,col);
	attron(A_BOLD);
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
	usleep(100000);
	
	attroff(A_BOLD);
	add_colour(1,col);
	typewriter(title2,120);
	
	usleep(500000);
	attrset(COLOR_PAIR(3));
	move(LINES/2+2,COLS/2-strlen(subtitle)/2);
	typewriter(subtitle,30);
	move(1,1);
	for(kk=1;kk<=LINES/2-2;kk++)
	{
		deleteln();
		refresh();
		usleep(20000);
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
		usleep(20000);
	}
	refresh();
	noecho();
	move(4,strlen(subtitle)/2+strlen(title1)/2+1+strlen(title2)/2);
	delch();
	insch(':');
	refresh();
}

//Bordered window with content
WINDOW *bt_win(char *title, int height, int width, int ypos,int xpos)
{	
	WINDOW *f_win;

	f_win = newwin(height,width,ypos,xpos);
	wbkgd(f_win,COLOR_PAIR(3));
	box(f_win, 0 , 0);		
	mvwprintw(f_win,1,width/2-strlen(title)/2,title);
	return f_win;
}

char *subject(int n) /*returns the subject to extract entries from*/
{
	FILE *f_sublist; 
	char *subj_buf=NULL; /*buffers must be null for getline to work*/
	size_t  subj_buf_size; /*Sizes of buffer necessary too*/

	char *sub_string; /*output and printed at top*/
	ssize_t subj_size;
	int kk=0;
	//Reading subject list
	f_sublist = fopen("subjects.conf", "r");
	subj_size = getline(&subj_buf,&subj_buf_size,f_sublist);

	while(subj_size>=0) /*getline fills subj_buf with line from file*/
	{
		if(kk==n)
			break;
		kk++;
		subj_size = getline(&subj_buf,&subj_buf_size,f_sublist);
	}
	
	//need to snip off the final character. Is strcpy necessary? 
	sub_string = (char *)malloc(strlen(subj_buf)*sizeof(char));
	strncpy(sub_string,subj_buf,strlen(subj_buf)-1);
	*(sub_string+strlen(subj_buf)-1)='\0';
	fclose(f_sublist);

	attron(A_BOLD);
	mvprintw(2,20,sub_string);
	attroff(A_BOLD);

	free(subj_buf);
	return(sub_string);
}

void ecount_update(int ent_count,int no_of_entries)
{
	move(3,19);
	clear_text(9);
	if(ent_count<10)
		mvprintw(3,21,"%d/%d",ent_count,no_of_entries);
	else if(ent_count < 100)
		mvprintw(3,20,"%d/%d",ent_count,no_of_entries);
	else
		mvprintw(3,19,"%d/%d",ent_count,no_of_entries);
}

//Updates the window with a content window, scm is scroll value 
void update_win(WINDOW *win,char *new_content, int scm)
{
	WINDOW *c_win;
	int height, width;
	int npl, nchars;
	int c, cmax, cmin;
	char ch;

	getmaxyx(win,height,width); 	/*measures size of window*/
	npl = width-4;			/*number of characters per line*/
	nchars=(height-3)*(width-4);    /*number of characters in the window*/
	cmax = min(strlen(new_content)-1,nchars); /*last char printed to window*/

	if(cmax == nchars) /*case where too much content for window*/
	{
		cmin = max(npl*scm,0); /*cmin is first char*/
	}
	else
		cmin =0;

	cmax = min(cmax +cmin,strlen(new_content)-1);/*last char calculated*/

	c_win = derwin(win,height-3,width-4,2,2);
	wbkgd(c_win,COLOR_PAIR(4));
	wclear(c_win);
	//wattron(c_win,A_BOLD);
	wmove(c_win,0,0);
	for(c=cmin;c<=cmax;c++)
	{
		ch = (char)*(new_content+c);
		waddch(c_win,ch);
	}
	//wattroff(c_win,A_BOLD);
	wrefresh(c_win);
	wrefresh(win);
}

//Fetches entry from the subject file
char *fetch_entry(char* sub,int entry_no, int line_count,int etype)
{
	FILE *sub_file;
	char *line_buffer = NULL; /*line buffer is filled with line from file using getline*/
	size_t lb_size;
	ssize_t line_size=0;
	int lvar = (entry_no-1)*5+2+etype;
	int kk;
	sub_file = fopen(sub,"r");
	//loop ends when correct line reached
	for(kk=0;kk<=lvar;kk++) 
	{
		line_size = getline(&line_buffer, &lb_size, sub_file);
	}
	fclose(sub_file);
	line_buffer[line_size-1]='\0';
	return(line_buffer);
}

//make_menu prints an instant of the window to mwin, and returns name of
//menu item for use in file path
char *make_menu(WINDOW *mwin,char *path_to_file,int item, int scl)
{
	FILE *menu_list; /*menu list stored in individual files*/
	int kk=0;
	int wh, ww;
	char *buf=NULL;
	char *sel_item;
	size_t buf_size; 
	ssize_t lsize;

	getmaxyx(mwin,wh,ww);
	size_t menu_item_size=(ww-1)*sizeof(char);
	char *menu_item = calloc(ww-1,sizeof(char));

	menu_list = fopen(path_to_file,"r");

	lsize = getline(&buf,&buf_size,menu_list);
	while(lsize >=0)	
	{
		strncpy(menu_item,buf,strlen(buf)-1);
		*(menu_item+strlen(buf)-1)='\0';/*no pesky newline character*/

	      	wmove(mwin,kk-scl,0);/*clear old menu item in this spot*/
		wclrtoeol(mwin);
		if(kk==item) /*highlight item selected, and copy for output*/
		{
			sel_item = (char *)malloc(menu_item_size);
			strcpy(sel_item,menu_item);
			wattron(mwin,A_REVERSE);
		}
		if(kk>=scl)/*only print the menu items that can fit in window*/
			mvwaddstr(mwin,kk-scl,1,menu_item);
		wattroff(mwin,A_REVERSE);
		lsize = getline(&buf,&buf_size,menu_list);
		if(kk>wh+scl)
			break;
		kk++;
	}
	fclose(menu_list);

	free(buf);
	free(menu_item);
	return(sel_item); /*memory freed in main*/
}

//refresh_menu responds to minput and refreshes the new menu
char *refresh_menu(WINDOW *menu_win,char* path_to_file,int m_input, int *mm, int *sv)
{
	int wh, ww, lc; /*need to know: window dims and number of lines for menu*/
	char *categ;

	lc = line_counter(path_to_file);
	getmaxyx(menu_win,wh,ww);

	switch(m_input) /* adjusts menu according to item selected and scroll value*/
	{
		case KEY_DOWN:
			*mm = *mm+1;
			if(*mm>=lc)
			{
				*mm=0;
				*sv=0;
			}
			if(lc<wh)
				*sv=0;
			else if(*mm>=wh-*sv)
				*sv = min(*sv+1,lc-wh);
			break;
		case KEY_UP:
			*mm = *mm-1;
			if(*mm<0)
			{
				*mm=lc-1;
				*sv=lc-wh;
			}
			if(lc<wh)
				*sv=0;
			else if(*sv>0 && *mm<*sv)
				*sv = *sv-1;
			break;
		case '\n':
			*mm = *mm;
			*sv = *sv;
			break;
	}

	categ =make_menu(menu_win,path_to_file, *mm,*sv);	
	return(categ); /*memory freed in main*/
}

//Navigator executes purpose of the program. Navigates arxiv entries
void navigator(WINDOW *twin,WINDOW *authwin,WINDOW *abswin,char *sub, int input,int *entry_no, int *abs_scrl)
{
	int no_of_entries;
	int line_count = line_counter(sub);
	no_of_entries = (line_count-1)/5;	/*Computing the number of entries*/

	//Entry data to display
	char *entry_arxivno, *entry_title, *entry_abstract, *entry_authors;

	ecount_update(*entry_no,no_of_entries); /*update the displayed entry number*/

	int ran; /*True when arxiv number needs to be refreshed*/

	switch(input) /*Needs to update abstract  sroll value, changes entry number*/
	{
		case KEY_LEFT:
			*entry_no = *entry_no-1;
			if(*entry_no<1)
				*entry_no=no_of_entries;
			entry_abstract = fetch_entry(sub,*entry_no,line_count,3);
			ran = 1;
			break;
		case KEY_RIGHT:
			*entry_no = *entry_no+1;
			if(*entry_no>no_of_entries)
				*entry_no=1;
			entry_abstract = fetch_entry(sub,*entry_no,line_count,3);
			ran = 1;
			break;
		case KEY_DOWN:
			*entry_no = *entry_no;
			entry_abstract = fetch_entry(sub,*entry_no,line_count,3);
			*abs_scrl = new_scroll_value(abswin,entry_abstract,*abs_scrl+1);
			ran = 0;
			break;
		case KEY_UP:
			*entry_no = *entry_no;
			entry_abstract = fetch_entry(sub,*entry_no,line_count,3);
			*abs_scrl = new_scroll_value(abswin,entry_abstract,*abs_scrl-1);
			ran = 0;
			break;
		case '0':
			entry_abstract = fetch_entry(sub,*entry_no,line_count,3);
			ran=1;
			break;
		default:
			entry_abstract = fetch_entry(sub,*entry_no,line_count,3);
			ran=0;
			break;
	}
	//entry data goes to respective strings
	entry_arxivno = fetch_entry(sub,*entry_no,line_count,0);
	entry_title = fetch_entry(sub,*entry_no,line_count,1);
	entry_authors = fetch_entry(sub,*entry_no,line_count,2);

	//refreshing displayed windows
	ecount_update(*entry_no,no_of_entries);
	update_win(twin,entry_title, *abs_scrl);
	update_win(authwin,entry_authors,*abs_scrl);
	update_win(abswin,entry_abstract,*abs_scrl);
	if(ran)
	{
		attron(A_BOLD);
		move(4,28);
		clear_text(10);
		move(4,28);
		typewriter(entry_arxivno,40);
		attroff(A_BOLD);
		ran=0;
	}
	wrefresh(twin);
	wrefresh(authwin);
	wrefresh(abswin);
	free(entry_arxivno);
	free(entry_title);
	free(entry_authors);
	free(entry_abstract);
}

void chosen_subjects(WINDOW *cho_win,int scl)
{
	FILE *sub_list;
	int kk=0;
	int wh, ww;
	char *buf=NULL;
	size_t buf_size; 
	ssize_t lsize;
	getmaxyx(cho_win,wh,ww);
	char *menu_item = calloc(ww-1,sizeof(char));
	sub_list = fopen("subjects.conf","r");
	lsize = getline(&buf,&buf_size,sub_list);
	while(lsize >=0)	
	{
		strncpy(menu_item,buf,strlen(buf)-1);
		*(menu_item+strlen(buf)-1)='\0';

	      	wmove(cho_win,kk-scl,0);
		wclrtoeol(cho_win);
		if(kk>=scl)
			mvwaddstr(cho_win,kk-scl,1,menu_item);
		wattroff(cho_win,A_REVERSE);
		lsize = getline(&buf,&buf_size,sub_list);
		if(kk>wh+scl)
			break;
		kk++;
	}
	fclose(sub_list);
	free(buf);
	free(menu_item);
}

void clear_sublist()
{
	FILE *sublist;
	sublist = fopen("subjects.conf","w");
	fclose(sublist);
}
