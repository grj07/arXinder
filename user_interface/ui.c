#include <ncurses.h> /*ncurses library, includes stdio.h*/
#include <math.h>    /*Floor and ceilings*/
#include <string.h>  /*string manipulations str...*/
#include <stdlib.h>  /*dynamic memory allocation malloc*/
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "gfuncs.h"
#include "pfuncs.h"

#define NTHREADS  2
#define VERSION "0.0.3"

int main(int argc, char *argv[])
{
	//declaring principle windows to display
	WINDOW *title_win, *auth_win, *abs_win; 
	char chsub[] = "s - change subjects";  
	char sor_inst[] = "Use backspace to reject, return to save";  

	//User input for entry navigator
	int input, mm, scrl, subq;


	//This needs to become an array of pointers to subject strings. 
	char *sub; /*the current subject area being explored*/
	size_t sub_size;



	//initialise ncurses mode
	initscr();			

	int col;
	col = colours(); /*col=0 is black and white mode*/
	keypad(stdscr, TRUE);		
	noecho();
	curs_set(0);

	while(!line_counter("subjects.conf")) 	
	{
		move(1,20);
		printw("Please choose at least one subject area");
		refresh();
		move(1,20);
		s_menu();
		clear_text(40);
	}



	;
	int ii, rc;
	pthread_t threads[NTHREADS];


	  /* threading title-screen with python script */

	rc = pthread_create(&threads[0], NULL, call_datprc,NULL);
	//Title screen on opening
	rc = pthread_create(&threads[1], NULL, title_screen,&col);
	
	/* wait for threads to finish */
	for (ii=0; ii<NTHREADS; ii++) 
	{
		rc = pthread_join(threads[ii], NULL);
	}


	add_colour(2,col);
	move(2,10);
	typewriter("v ",100);
	typewriter(VERSION,100);
	mvprintw(4,COLS-strlen(chsub),chsub);
	mvprintw(3,COLS-strlen(sor_inst),sor_inst);
	rem_colour(2,col);
	refresh();

	//entries to be displayed here
	title_win = bt_win("Title",8,COLS/2-2,5,2);
	auth_win = bt_win("Authors",8,COLS/2-2,5,COLS/2+1);
	abs_win = bt_win("Abstract", LINES-13,COLS-4,13,2);

	wattrset(abs_win,COLOR_PAIR(2));
	mvwaddstr(abs_win,1,COLS-28,"Use arrowkeys to scroll");
	wrefresh(title_win);
	wrefresh(auth_win);
	wrefresh(abs_win);

	//Python script updates subject files
	
	//puts subjects in an array 
	int nsubs = line_counter("subjects.conf");
	char *subject_array[nsubs];
	char *line_buffer=NULL;
	size_t lbsize;
	ssize_t line_size;

	FILE *fp;
	fp= fopen("subjects.conf","r");
	ii=0;
	line_size = getline(&line_buffer,&lbsize,fp);
	subject_array[ii] = malloc((strlen(line_buffer))*sizeof(char));
	strncpy(subject_array[ii],line_buffer,strlen(line_buffer)-1);
	*(subject_array[ii]+strlen(line_buffer)-1)='\0';
	while(line_size>=0)
	{
		ii++;
		line_size = getline(&line_buffer,&lbsize,fp);	
		subject_array[ii] = malloc((strlen(line_buffer))*sizeof(char));
		strncpy(subject_array[ii],line_buffer,strlen(line_buffer)-1);
		*(subject_array[ii]+strlen(line_buffer)-1)='\0';
	}
	fclose(fp);

	sub = subject_array[0];

	//Subject area metadata
	int *line_count[nsubs-1];
	int *no_of_entries[nsubs-1];

	update_subs(0,subject_array ,nsubs);


	refresh();
	flushinp(); /*Clears the keyboard buffer so any key hit during title screen not input*/

	mm=1; /*start on first entry, scroll set to top, meaningless input*/
	scrl=0;
	input = '0';
	int sub_number = 0; /*subject number for subject_array*/
	do{
		if(input == 's') 
			s_menu();	/*Subject menu defined in pfuncs.c */
		if(!line_counter("subjects.conf")) 	
		{
			move(1,20);
			printw("Please choose at least one subject area");
			refresh();
			move(1,20);
			napms(2000);
			clear_text(30);
			input='s';
			continue;
		}
	
		touchwin(title_win);
		touchwin(auth_win);
		touchwin(abs_win);
	
		//Navigator explores entries, defined in pfuncs.c
		navigator(title_win,auth_win,abs_win,sub,input,&mm,&scrl); 
	
		input = getch();
	}while(input!='q');



	endwin();			/* End curses mode		  */

	for(ii=0;ii<=nsubs;ii++)
	{
		free(subject_array[ii]);
	}
	free(line_buffer);
	return 0;
}
