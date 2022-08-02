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

void bomb(char *msg)
	//Quits program and prints message
{
	endwin();
	puts(msg);
	exit(1);
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
	init_pair(5,COLOR_RED,LYELLOW);
	}	
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

int line_counter(char *filename)
{
	FILE *f = fopen(filename, "r"); /*opening the first subject file*/
	int total_lines=0;
	char * line;
	size_t lineb_size;
	int line_size = getline(&line, &lineb_size, f);
	while(line_size>=0) 
	{
		line_size = getline(&line, &lineb_size, f);
		total_lines++;				 
	}
	fclose(f);
	return(total_lines);
}

void clear_text(int length)
	//erases from cursor position
{
	int kk=0;
	for(kk=0;kk<=length;kk++)
		addch(' ');
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

int new_scroll_value(WINDOW *win,char *content,int scv)
{
	int  height,width;
	float scl;
	getmaxyx(win,height,width);
	scl = max(0,scv);
	scl = min(strlen(content)/(width-4)-(height-4),scl);
	return((int)scl);
}

void update_win(WINDOW *win,char *new_content, int scm)
{
	WINDOW *c_win;
	int height, width;
	int npl, nchars;
	int c, cmax, cmin;
	char ch;
	getmaxyx(win,height,width);
	npl = width-4;
	nchars=(height-3)*(width-4);
	cmax = min(strlen(new_content)-1,nchars);
	if(cmax == nchars)
	{
		cmin = max(npl*scm,0);
	}
	else
		cmin =0;
	cmax = min(cmax +cmin,strlen(new_content)-1);
	c_win = derwin(win,height-3,width-4,2,2);
	wbkgd(c_win,COLOR_PAIR(4));
	wclear(c_win);
//	scrollok(c_win,TRUE);
	wmove(c_win,0,0);
//	wprintw(c_win,"This is the string length :%d",cmax);
	for(c=cmin;c<=cmax;c++)
	{
		ch = (char)*(new_content+c);
		waddch(c_win,ch);
	}
	wrefresh(c_win);
	wrefresh(win);
}

void pull_datum(char **datum,size_t *datum_size,char *buffer)
{
	*datum_size = strlen(buffer);
	*datum = (char *)malloc(*datum_size);
	strncpy(*datum,buffer,*datum_size-1);
	*(*datum +*datum_size-1) = '\0';
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

void ecount_update(int ent_count,int no_of_entries)
{
	if(ent_count<10)
		mvprintw(3,21,"%d/%d",ent_count,no_of_entries);
	else if(ent_count < 100)
		mvprintw(3,20,"%d/%d",ent_count,no_of_entries);
	else
		mvprintw(3,19,"%d/%d",ent_count,no_of_entries);
}

void make_mmenu(WINDOW *mwin,int item)
{
	char inp;
	int kk;
	char mmenu[8][50] ={
			"Computer Science",
			"Economics",
			"Electrical Engineering and Systems Science",
			"Mathematics",
			"Physics",
			"Quantitative Biology",
			"Quantitative Finance",
			"Statistics"
			};
	for(kk=0;kk<8;kk++)
	{
		if(kk==item)
		{
			wattron(mwin,COLOR_PAIR(5));
		}
		mvwaddstr(mwin,kk+1,1,mmenu[kk]);
		wattroff(mwin,COLOR_PAIR(5));
	}
	wrefresh(mwin);
}

void make_smenu(WINDOW *mwin,char *categ,int sitem)
{
	char buf;
	size_t buf_size;
	ssize_t line_size;
	char file_string[60]; 
	char folder[] = "subjects/";
	int i;
	for(i=0;i<strlen(folder);i++)
	{
		file_string[i]= folder[i];
	}
	for(i=strlen(folder);i<=strlen(folder)+strlen(categ);i++)
	{
		file_string[i]= categ[i-strlen(folder)];
	}
	FILE *cat_file; 
	cat_file = fopen(file_string, "r");
	buf = fgetc(cat_file);
	while(buf!=EOF)
	{
		wprintw(mwin,"%c",buf);
		buf = fgetc(cat_file);
	}
//	line_size = getline(&buf, &buf_size, cat_file);
//	/*while loop navigates through entries*/
//	while(line_size >= 0)
//	{
//		line_size = getline(&buf, &buf_size, cat_file);
//		addstr(buf);
//	}
//	char inp;
//	int kk;
//	for(kk=0;kk<8;kk++)
//	{
//		if(kk==item)
//			wattron(mwin,COLOR_PAIR(5));
//		mvwaddstr(mwin,kk+1,1,mmenu[kk]);
//		wattroff(mwin,COLOR_PAIR(5));
//	}
	addstr(file_string);
	fclose(cat_file);
	wrefresh(mwin);
}

void cat_menu(WINDOW *menu_win,int m_input, int *mm)
{
	char *categ;
	switch(m_input)
	{
		case KEY_DOWN:
			*mm = *mm+1;
			if(*mm>=8)
				*mm=0;
			make_mmenu(menu_win, *mm);	
			wrefresh(menu_win);
			break;
		case KEY_UP:
			*mm = *mm-1;
			if(*mm<0)
				*mm=7;
			make_mmenu(menu_win, *mm);	
			wrefresh(menu_win);
			break;
	}
}

//int fetch_entry(FILE *f,char *arxiv_no,char *title,char *authors,char *abstract)
//{
//	size_t lb_size, line_size;
//	char *line_buf;
//	line_size = getline(&line_buf, &lb_size, f)
//	if( swtit ==1){
//		entry_title = (char *)malloc(strlen(line_buf)-1);
//		//Copies from buffer, stores to allocated memory
//		strncpy(entry_title,line_buf,strlen(line_buf)-1);
//		swtit=0;
//		swauth=1;
//	}
//	else if( swauth ==1){
//		entry_authors = (char *)malloc(strlen(line_buf)-1);
//		strncpy(entry_authors,line_buf,strlen(line_buf)-1);
//		swauth=0;
//		swabs=1;
//	}
//	else if( swabs ==1){
//		entry_abstract = (char *)malloc(strlen(line_buf)-1);
//		strncpy(entry_abstract,line_buf,strlen(line_buf)-1);
//		swabs=0;
//		//At this point the entire entry has been copied, 
//		//and can be printed as the contents
//		//of the window
//		update_win(title_win,entry_title);
//		update_win(auth_win,entry_authors);
//		update_win(abs_win,entry_abstract);
//		move(4,27);
//		clear_text(10);
//		move(4,27);
//		typewriter(entry_arxivno,40);
//		input = getch();
//		if(input ==KEY_RIGHT)
//		{
//			line_count++;
//			swarx = line_count-1;
//			swarx = swarx % 5;
//			ent_count++;
//			line_size = getline(&line_buf, &line_buf_size, fp);
//			seekch[line_count+1] = line_size;
//			continue;
//		}	
//		else if(input ==KEY_LEFT)
//		{
//			if(ent_count==1)
//			{
//				break;
//			}
//			else{
///*this code counts how */		totch=seekch[line_count+1]+
///*many characters the cursor*/			seekch[line_count]+seekch[line_count-1]+
///*skips back to the previous entry*/		seekch[line_count-2]+seekch[line_count-3]+
//						seekch[line_count-4];
//					line_count=line_count-5;
//					totch=totch +seekch[line_count]+seekch[line_count-1]+
//						seekch[line_count-2]+seekch[line_count-3]+
//						seekch[line_count-4];
//					line_count=line_count-5;
//					fseek(fp,-totch,SEEK_CUR);
//					line_size = getline(&line_buf, &line_buf_size, fp);
//					line_count++;
//					swarx = line_count-1;
//					swarx = swarx % 5;
//					ent_count = ent_count-1;
//					line_size = getline(&line_buf, &line_buf_size, fp);
//					seekch[line_count+1] = line_size;
//					continue;
//				}
//			}
//			else if(input =='s')
//			{
//				menu_win = newwin(0,0,5,0);
//				mvwprintw(menu_win,1,1,"Welcome to the menu!");
//				wrefresh(menu_win);
//				while (getch() != 'q')
//					;
//				wclear(menu_win);
//				wrefresh(menu_win);
//				title_win = bwinc("Title",entry_title, 8,COLS/2-2,5,2);
//				auth_win = bwinc("Authors", entry_authors,8,COLS/2-2,5,COLS/2+1);
//				abs_win = bwinc("Abstract",entry_abstract, LINES-13,COLS-2,13,2);
//
//			}
//			else
//			{
//				break;
//			}
//		}
//		else if( swarx==0){
//			entry_arxivno = (char *)malloc(strlen(line_buf)-1);
//			strncpy(entry_arxivno,line_buf,strlen(line_buf)-1);
//			entry_arxivno[strlen(line_buf)-1]='\0';
//			swtit = 1;
//	}
//	line_count++;
//	swarx = line_count-1;
//	swarx = swarx % 5;
//	line_size = getline(&line_buf, &line_buf_size, fp);
//	seekch[line_count+1] = line_size;
//	}
//	return(0);
//}
