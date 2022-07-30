#include <ncurses.h> /*ncurses library, includes stdio.h*/
#include <math.h>    /*Floor and ceilings*/
#include <string.h>  /*string manipulations str...*/
#include <stdlib.h>  /*dynamic memory allocation malloc*/
#include <menu.h>    /*menu library for options (e.g. which subject area to query)*/
#include "gfuncs.h"

#define FILENAME "prcd.dat" /*static file name for now. Likely to need dynamic later*/
#define VERSION "0.0.3"

int main(int argc, char *argv[])
{
	//Window declarations
	WINDOW *my_win, *title_win, *auth_win, *abs_win;
	char chsub[] = "s - subjects";
	char *sub;
	//Character read from user
	int input;

	//Declarations for reading entry data from file
	char *line_buf = NULL;
	char *subj_buf;
	char *entry_arxivno, *entry_title, *entry_abstract, *entry_authors;
	//Array to store line sizes to seek backwards to previous entries
	size_t seekch[10000]; /*A more advanced code would detect the 
				number of lines in the file */
	size_t line_buf_size = 0;
	size_t subj_buf_size = 0;
	int line_count = 0, totch=0, ent_count=1;
	//Switches which track what data type the buffer is reading
	int swarx = 1, swtit = 0, swauth = 0, swabs = 0;
	ssize_t line_size, subj_size;
	//Start ncurses
	initscr();			

	keypad(stdscr, TRUE);		
	colours();

	//Title screen on opening
	title_screen();
	move(2,10);
	attrset(COLOR_PAIR(2));
	typewriter("v ",100);
	typewriter(VERSION,100);

	title_win = bwinc("Title","", 8,COLS/2-2,5,2);
	napms(100);
	auth_win = bwinc("Authors", "",8,COLS/2-2,5,COLS/2+1);
	napms(100);
	abs_win = bwinc("Abstract","", LINES-13,COLS-2,13,2);
	napms(100);
	flushinp();
	//Window contents now
	/* Open the file for reading */
	FILE *f_subject_list = fopen("subjects.conf", "rw");
	subj_size = getline(&subj_buf,&subj_buf_size,f_subject_list);
	sub = (char *)malloc(strlen(subj_buf)-1);
	strncpy(sub,subj_buf,strlen(subj_buf)-1);
	fclose(f_subject_list);
	curs_set(1);
	mvprintw(2,20,sub);
	
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
			//At this point the entire entry has been copied, 
			//and can be printed as the contents
			//of the window
			update_win(title_win,entry_title);
			update_win(auth_win,entry_authors);
			update_win(abs_win,entry_abstract);
			input = getch();
			if(input ==KEY_RIGHT)
			{
				line_count++;
				swarx = line_count-1;
				swarx = swarx % 5;
				ent_count++;
				continue;
			}	
			else if(input ==KEY_LEFT)
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

