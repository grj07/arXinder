#include <Python.h>
#include <ncurses.h> /*ncurses library, includes stdio.h*/
#include <math.h>    /*Floor and ceilings*/
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>
#include "gfuncs.h"
#include "pfuncs.h"

#define NTHREADS  2
#define VERSION "0.0.3"

//This function runs Python script, threads with the user interface
void *call_datprc(void *pyscript)
{
	char *scriptname = (char *)pyscript;
	FILE* fp;

	Py_Initialize();

	fp = fopen(scriptname, "r");
	PyRun_SimpleFile(fp, scriptname);

	Py_Finalize();
	return NULL;
}

void *title_screen(void * colo)
{
	int col = *(int *)colo;
	char title1[]="ar";
	char title2[]="inder";
	char subtitle[]="sort and file the new papers from the arXiv.";
	int disp = (strlen(title1)+strlen(title2)+1)/2;
	int kk;

	curs_set(0);
	move(LINES/2,COLS/2-disp);
	add_colour(1,col);
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
	add_colour(3,col);
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

void *user_interface(void * colo)
{
	int col = *(int *)colo;
	//declaring principle windows to display
	WINDOW *title_win, *auth_win, *abs_win; 
	char sub_inst[] = "s - change subjects";  
	char sor_inst[] = "Use backspace to reject, return to save";  

	//User input for entry navigator
	int input, mm, scrl, subq;

	//int col = *(int *)colo;

	//This needs to become an array of pointers to subject strings. 
	char *sub; /*the current subject area being explored*/
	size_t sub_size;



	//initialise ncurses mode
	mvprintw(4,COLS-strlen(sub_inst),sub_inst);
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
	int ii=0;
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


	update_subs(0,subject_array ,nsubs);


	refresh();
	flushinp(); /*Clears the keyboard buffer so any key hit during title screen not input*/

	mm=1; /*start on first entry, scroll set to top, meaningless input*/
	scrl=0;
	input = '0';
	int sub_no = 0; /*subject number for subject_array*/
	bool chsub=0;
	do{
		if(input == 's') 
			s_menu();	/*Subject menu defined in pfuncs.c */
		//Need to reload subjects if they change.
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
		if(chsub)
		{
			sub_no++;
			if(sub_no>= nsubs)
			{
				break;
			}
			chsub =0;
			mm=1;
			update_subs(sub_no,subject_array ,nsubs);
		}
		sub = subject_array[sub_no];
		chsub = navigator(title_win,auth_win,abs_win,sub,&input,&mm); 
	}while(input!='q');



	endwin();	/* quit ncurses */

	for(ii=0;ii<nsubs;ii++)
	{
		free(subject_array[ii]);
	}
	free(line_buffer);
}

int main()
{
	int ii, rc, col;

        setlocale(LC_ALL, "");
	initscr();			

	//col = colours(); 
	col = 0; /*col=0 is black and white mode*/
	keypad(stdscr, TRUE);		
	noecho();
	curs_set(0);

	while(!(line_counter("subjects.conf"))) 	
	{
		move(1,20);
		printw("Please choose at least one subject area");
		refresh();
		move(1,20);
		s_menu();
		clear_text(40);
	}

	//Title screen on opening
	char fent[] = "fent.py";
	char rent[] = "rent.py";
	pthread_t threads[NTHREADS];
	rc = pthread_create(&threads[0], NULL, call_datprc,fent);
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

	pthread_t uthreads[NTHREADS];
	rc = pthread_create(&uthreads[0], NULL, call_datprc,rent);
	rc = pthread_create(&uthreads[1], NULL, user_interface,&col);
	
	/* wait for threads to finish */
	for (ii=0; ii<NTHREADS; ii++) 
	{
		rc = pthread_join(uthreads[ii], NULL);
	}


	return 0;
}
