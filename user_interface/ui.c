#include <ncurses.h> /*ncurses library for "WINDOW" etc*/
#include <math.h>    /*Floor and ceilings*/
#include <string.h>  /*string manipulations str...*/
#include <stdio.h>   /*file manipulation*/
#include <stdlib.h>  /*dynamic memory allocation malloc*/
#include <menu.h>    /*menu library for options (e.g. which subject area to query)*/

#define FILENAME "prcd.dat" /*static file name for now. Likely to need dynamic later*/

//Declarations of functions: main code now, functions below.
WINDOW *fixed_window(int bo, char title[], int hh, int ww, int sh, int sw);
WINDOW *cont_window(int bo, char content[], int hh, int ww, int sh, int sw);

	//Framework of boxes for title, authors, abstract 
int main(int argc, char *argv[])
{
	//Window declarations
	WINDOW *my_win, *fwin1, *fwin2, *fwin3, *fwin1_cont, *fwin2_cont, *fwin3_cont;
	//Fixed strings
	char main_title[]="Xiview: an arXiv viewer (version 0.0.2)";
	char nav_guide[]="Use arrow keys <- , -> to navigate through entries.";
	char sbj_guide[]="s - choose subject areas";
	//Character read from user
	int ch;

	//Declarations for reading entry data from file
	//Note: Pointers for storing strings, memory allocated later in loop when size can be read from line
	//buffer
	char *line_buf = NULL;
	char *entry_arxivno, *entry_title, *entry_abstract, *entry_authors;
	//Array to store line sizes to seek backwards to previous entries
	size_t seekch[10000]; /*Should really detect the number of lines in the file but oh well*/
	size_t line_buf_size = 0;
	int line_count = 0, totch=0, ent_count=1;
	//Switches which track what data type the buffer is reading
	int swarx = 1, swtit = 0, swauth = 0, swabs = 0;
	ssize_t line_size;
	//Start ncurses
	initscr();			
	cbreak();			
  					
	keypad(stdscr, TRUE);		

	//Generating title and windows
	int title_hpos=floor(COLS/2-strlen(main_title)/2);
	mvprintw(1,title_hpos,main_title);
	mvprintw(2,1,nav_guide);
	int sbjguide_hpos=COLS-strlen(sbj_guide);
	mvprintw(2,sbjguide_hpos,sbj_guide);
	refresh();
	int etitle_size[]={floor(LINES/3)-1,floor(COLS/2)}, etitle_pos[]={5,1};
	fwin1 = fixed_window(1,"Title", etitle_size[0],etitle_size[1],etitle_pos[0],etitle_pos[1]);
	int eauths_size[2]={floor(LINES/3)-1, floor(COLS/2)-2}, eauths_pos[2]={5, ceill(COLS/2)+1};
	fwin2 = fixed_window(1,"Authors", eauths_size[0],eauths_size[1],eauths_pos[0],eauths_pos[1]);
	int eabs_size[2]={2*floor(LINES/3)-4, COLS-2}, eabs_pos[2]={ceill(LINES/3)+4, 1};
	fwin3 = fixed_window(1,"Abstract", eabs_size[0],eabs_size[1],eabs_pos[0],eabs_pos[1]);
	//Window contents now
	/* Open the file for reading */
	FILE *fp = fopen(FILENAME, "r");
	
	/* Get the first line of the file. */
	line_size = getline(&line_buf, &line_buf_size, fp);
	seekch[0] = line_size;
	while(line_size >= 0){
		line_size = getline(&line_buf, &line_buf_size, fp);
		seekch[line_count+1] = line_size;
		if( swtit ==1){
			//Allocates memory according to string length of data, minus one to skip 
			//new line character
			entry_title = (char *)malloc(strlen(line_buf)-1);
			//Copies from buffer, stores to allocated memory
			strncpy(entry_title,line_buf,strlen(line_buf)-1);
			swtit=0;
			swauth=1;
		}
		else if( swauth ==1){
			entry_authors = (char *)malloc(strlen(line_buf)-1);
			strncpy(entry_authors,line_buf,strlen(line_buf)-1);
			swauth=0;
			swabs=1;
		}
		else if( swabs ==1){
			entry_abstract = (char *)malloc(strlen(line_buf)-1);
			strncpy(entry_abstract,line_buf,strlen(line_buf)-1);
			swabs=0;
			//At this point the entire entry has been copied, and can be printed as the contents
			//of the window
			fwin1_cont = cont_window(0,entry_title, etitle_size[0]-4, etitle_size[1]-2, etitle_pos[0]+2, etitle_pos[1]+1);
			fwin2_cont = cont_window(0,entry_authors, eauths_size[0]-4,eauths_size[1]-2,eauths_pos[0]+2,eauths_pos[1]+1);
			fwin3_cont = cont_window(0,entry_abstract, eabs_size[0]-4,eabs_size[1]-6,eabs_pos[0]+2,eabs_pos[1]+3);
			refresh();
			ch = getch();
			if(ch ==KEY_RIGHT)
			{
				line_count++;
				swarx = line_count-1;
				swarx = swarx % 5;
				ent_count++;
				continue;
			}	
			else if(ch ==KEY_LEFT)
			{
				if(ent_count==1)
				{
					break;
				}
				else{
					//To get back to previous entry, I need to calculate how many 
					totch=seekch[line_count+1]+seekch[line_count]+seekch[line_count-1]+
						seekch[line_count-2]+seekch[line_count-3]+seekch[line_count-4];
					line_count=line_count-5;
					totch=totch +seekch[line_count]+seekch[line_count-1]+
						seekch[line_count-2]+seekch[line_count-3]+seekch[line_count-4];
					line_count=line_count-5;
					fseek(fp,-totch,SEEK_CUR);
					line_size = getline(&line_buf, &line_buf_size, fp);
					line_count++;
					swarx = line_count-1;
					swarx = swarx % 5;
					ent_count = ent_count-1;
					continue;
				}
			}
			else
			{
				break;
			}
		}
		else if( swarx==0){
			entry_arxivno = (char *)malloc(strlen(line_buf)-1);
			strncpy(entry_arxivno,line_buf,strlen(line_buf)-1);
			swtit = 1;
	}
	line_count++;
	swarx = line_count-1;
	swarx = swarx % 5;
	}
	fclose(fp);
	endwin();			/* End curses mode		  */
	return 0;
}

//Window generating functions
WINDOW *fixed_window(int bo, char title[], int hh, int ww, int sh, int sw)
{	
	WINDOW *f_win;

	f_win = newwin(hh,ww,sh,sw);
	if(bo == 1)
	{
	box(f_win, 0 , 0);		
	}					 
//	mvwprintw(f_win,2,1,content);
	mvwprintw(f_win,1,ww/2-strlen(title)/2,title);
	wrefresh(f_win);		/* Show that box 		*/

	return f_win;
}

WINDOW *cont_window(int bo, char content[], int hh, int ww, int sh, int sw)
{	
	WINDOW *f_win;

	f_win = newwin(hh,ww,sh,sw);
	if(bo == 1)
	{
	box(f_win, 0 , 0);		
	}					 
	mvwprintw(f_win,2,1,content);
//	mvwprintw(f_win,1,ww/2-strlen(title)/2,title);
	wrefresh(f_win);		/* Show that box 		*/

	return f_win;
}
