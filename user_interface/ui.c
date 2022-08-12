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
	FILE *sub_file;
	//declaring principle windows to display
	WINDOW *title_win, *auth_win, *abs_win, *menu_win; 
	//declaring subwindows 
	WINDOW *cat_win, *sub_win, *cho_win; 
	char chsub[] = "s - change subjects";  /*instructions*/
	char smsub[] = "c - clear subjects";  

	//User input for entry navigator
	int input, mm, scrl;

	//User input for menu system
	int cm = 0,csv = 0, sm = 0, ssv = 0;
	char *cat, *sub, *sbj;
	char *path = malloc(60*sizeof(char));
	char *tpath = malloc(60*sizeof(char));
	size_t sub_size;


	if(!line_counter("subjects.conf"))
		bomb("Error, no subject file");
	

	//initialise ncurses mode
	initscr();			

	int col;
	col = colours(); /*col=0 is black and white mode*/

	keypad(stdscr, TRUE);		
	noecho();
	curs_set(0);

	//Title screen on opening
	;
	int ii, rc;
	pthread_t threads[NTHREADS];


	  /* spawn the threads */
	rc = pthread_create(&threads[0], NULL, call_datprc,NULL);
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
	rem_colour(2,col);
	refresh();

	//entries to be displayed here
	title_win = bt_win("Title",8,COLS/2-2,5,2);
	auth_win = bt_win("Authors",8,COLS/2-2,5,COLS/2+1);
	abs_win = bt_win("Abstract", LINES-13,COLS-4,13,2);
	menu_win=bt_win("Subject Settings",LINES-4,COLS-4,5,2);

	wattrset(abs_win,COLOR_PAIR(2));
	mvwaddstr(abs_win,1,COLS-28,"Use arrowkeys to scroll");
	wrefresh(title_win);
	wrefresh(auth_win);
	wrefresh(abs_win);

	//Python script updates subject files
	
	//check and display name of subject area
	sub = subject(0);
	//Need to write a program which fills an array with the relevant subjects, and
	//displays them appropriately across the top.

	refresh();
	flushinp(); /*Clears the keyboard buffer so any key hit during title screen not input*/

	mm=1; /*start on first entry, scroll set to top, meaningless input*/
	scrl=0;
	input = '0';
	do	
	{
		//put in pfuncs --------------------------------------------------------------------------
		if(input == 's') /*Subject menu begins */
		{
			menu_win=bt_win("Subject Settings",LINES-5,COLS-4,5,2);
			wbkgd(menu_win,COLOR_PAIR(2));
			mvwaddstr(menu_win,1,COLS-25,smsub);
			wrefresh(menu_win);
			wbkgd(menu_win,COLOR_PAIR(4));
			cat_win = derwin(menu_win,9,45,2,1); 
			sub_win = derwin(menu_win,LINES-8,COLS-60,2,48);
			cho_win = derwin(menu_win,LINES-20,45,14,1);
			chosen_subjects(cho_win,0);
		
			wrefresh(cat_win);
			wrefresh(cho_win);
			cat = make_menu(cat_win,"cat_list",cm,csv);
			wrefresh(cat_win);
			input = getch();
			while(input!='q') /*choose category*/
			{	
				free(cat);
				cat = refresh_menu(cat_win,"cat_list",input,&cm,&csv);

				if(input == 'c')
				{
					clear_sublist();
					wclear(cho_win);
					chosen_subjects(cho_win,0);
					wrefresh(cho_win);
				}
				if(input=='\n')/*category selected, choose subject*/
				{
					
					free(path);
					path = join_strings("subjects/",cat);

					//fetch array with subject settings
					free(tpath);
					tpath = join_strings("subjects/",cat);

					sbj = make_menu(sub_win,path,sm,ssv);
					wrefresh(sub_win);
					input = getch();
					while(input!='q')
					{
						if(input == 'c')
						{
							clear_sublist();
							wclear(cho_win);
							chosen_subjects(cho_win,0);
							wrefresh(cho_win);
						}
						if(input == 'c')
							clear_sublist();
						if(input == '\n')/*subject selected, add to list*/
						{
							sub_file=fopen("subjects.conf","a");
							fprintf(sub_file,"%s\n",strtok(sbj ," ,"));
							fclose(sub_file);
							chosen_subjects(cho_win,0);
							wrefresh(cho_win);
						}
						free(sbj);
						sbj = refresh_menu(sub_win,path,input,&sm,&ssv);
						wrefresh(sub_win);
						input = getch();
					}
					free(sbj);
				}
				sm = 0;
				ssv = 0;
				wclear(sub_win);
				wrefresh(sub_win);
				wrefresh(cat_win);
				input = getch();
			}
			free(cat);
			cm = 0;			/*resets menu position and scroll*/
			csv = 0;
			wclear(menu_win);
			wrefresh(menu_win);
			delwin(menu_win);
		}/*Subject menu ends */
	//pfuncs ends here ---------------------------------------------------------------------------
	
	//Can't leave menu unless subjects have been chosen
	if(line_counter("subjects.conf")==0) 	
	{
		move(1,20);
		printw("Choose at least one subject");
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

	free(path);
	free(tpath);
	free(sub);
	return 0;
}
