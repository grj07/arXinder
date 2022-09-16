/* 
                             ******************* 
******************************* C SOURCE FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : ArXinder                                                    **  
** filename  : ui.c                                                        ** 
** version   : 0.1                                                         ** 
** date      : 16 Sep 2022             		                           ** 
**                                                                         ** 
***************************************************************************** 
**                                                                         ** 
** Code author: Guy R. Jehu - grj90@pm.me                                  ** 
**                                                                         ** 
***************************************************************************** 
 
VERSION HISTORY: 
---------------- 
 
Version     : 0.1 
Date        : 16 Sep 2022
Description : Original version. 
 
*/ 
 
/****************************************************************************/
/**                                                                        **/
/**                     MODULES USED                                       **/
/**                                                                        **/
/****************************************************************************/
#include "ui.h"

/****************************************************************************/
/**                                                                        **/
/**                     LOCAL FUNCTIONS                                    **/
/**                                                                        **/
/****************************************************************************/
void* call_datprc(void* pyscript){
	char* scriptname = (char* )pyscript;
	FILE* fp;

	Py_Initialize();

	fp = fopen(scriptname, "r");
	PyRun_SimpleFile(fp, scriptname);

	Py_Finalize();
	fclose(fp);
	return NULL;
}

//Title screen: animation while first subject entries load
void* title_screen(void* col){
	bool colours = *(int*)col;
	char title1[] = "ar", title2[]= "inder" ;
	char subtitle[]="sort and file the new papers from the arXiv.";
	int disp = (strlen(title1)+strlen(title2)+1)/2;
	int i;

	curs_set(0);
	move(LINES/2,COLS/2-disp);
	addColour(1,col);
	typewriter(title1,120);
	refresh();
	addColour(2,col);
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
	
	//Subtitle
	usleep(500000);
	add_colour(3,col);
	move(LINES/2+2,COLS/2-strlen(subtitle)/2);
	typewriter(subtitle,30);
	move(1,1);

	//Delete lines gradually
	for(i=1;i<=LINES/2-2;i++){
		deleteln();
		refresh();
		usleep(20000);
	}
	//Delete columns gradually
	for(kk=1;kk<=COLS/2-disp-1;kk++){
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

void make_title(bool colours){
	char title[]="arXinder";
	char subtitle[]="new papers from the arXiv:";

	add_colour(1,colours);
	mvaddstr(2,0,title);
	add_colour(2,colours);
	mvaddch(2,2,'X');
	mvaddch(1,1,'\\');
	mvaddch(3,3,'\\');
	mvaddch(1,3,'/');
	mvaddch(3,1,'/');
	add_colour(3,colours);
	mvaddstr(4,0,subtitle);

	curs_set(0);
	refresh();
	noecho();
}

int resume(int*  sub_no){
	FILE *fconf;
	FILE *fscript;
	int res=0;

	Py_Initialize(); /*should be program-wide*/

	fscript = fopen(RESUME_SCRIPT, "r");
	PyRun_SimpleFile(fscript,RESUME_SCRIPT);
	Py_Finalize();
	fclose(fscript);

	fconf = fopen(RESUME_FILE,"r");
	fscanf(fconf,"%d %d", &res, sub_no);
	fclose(fconf);
	return(res);
}

void run_arxinder(int colours, int* subNo){
	//declaring principle windows to display
	WINDOW* title_win;
       	WINDOW* auth_win; 
	WINDOW* abs_win; 
	//Instructions displayed
	char sub_inst[] = "s - change subjects";  
	char sor_inst[] = "Use backspace to reject, return to save, u to undo";  
	char qui_inst[] = "press q to quit";  
	char temp[] = "state/cursub";

	mvprintw(4,COLS-strlen(sub_inst),sub_inst);
	mvprintw(3,COLS-strlen(sor_inst),sor_inst);
	mvprintw(1,COLS-strlen(qui_inst),qui_inst);
	rem_colour(2,col);
	refresh();

	//User input for entry navigator
	int input, scrl, subq;

	//This needs to become an array of pointers to subject strings. 
	char* sub; /*the current subject area being explored*/
	size_t sub_size;

	//entries to be displayed here
	title_win = bt_win("Title",8,COLS/2-2,5,2);
	auth_win = bt_win("Authors",8,COLS/2-2,5,COLS/2+1);
	abs_win = bt_win("Abstract", LINES-13,COLS-4,13,2);

	wattrset(abs_win,COLOR_PAIR(2));
	wrefresh(title_win);
	wrefresh(auth_win);
	wrefresh(abs_win);

	
	//Subjects in an array to be loaded into array of strings
	int nSubs = lineCounter(SUBJECTS_FILE);
	char* subjectArray[nSubs];
	char* lineBuffer=NULL;
	size_t lineBuffersize;
	ssize_t lineSize;

	FILE* fp;
	fp= fopen(SUBJECTS_FILE,"r");
	int ii=0;
	lineSize = getline(&line_buffer,&lbsize,fp);
	subject_array[ii] = malloc((strlen(line_buffer))*sizeof(char));
	strncpy(subject_array[ii],line_buffer,strlen(line_buffer)-1);
	*(subject_array[ii]+strlen(line_buffer)-1)='\0';
	while(line_size>=0){
		ii++;
		line_size = getline(&line_buffer,&lbsize,fp);	
		subject_array[ii] = malloc((strlen(line_buffer))*sizeof(char));
		strncpy(subject_array[ii],line_buffer,strlen(line_buffer)-1);
		*(subject_array[ii]+strlen(line_buffer)-1)='\0';
	}
	fclose(fp);



	refresh();
	flushinp(); /*Clears the keyboard buffer so any key hit during title screen not input*/

	scrl=0;
	input = '0';
	bool chsub=0;
	bool acc = 0;
	int rc; /*For the threads*/
	nav_arg_struct* args = malloc(sizeof(nav_arg_struct));
	args->twin = title_win;
	args->awin = auth_win;
	args->abwin = abs_win;
	args->inp = &input;
	args->change_subject = &chsub;

	chsub = 0;
	do{
		if(input == 's'){
			if(s_menu()) 	/*Subject menu defined in pfuncs.c */
				bomb("Please restart program to enact subject changes");
		}
		//Need to reload subjects if they change.
		if(!line_counter("config/subjects.conf")) {
			move(1,30);
			printw("**Please choose at least one subject area**");
			refresh();
			move(1,30);
			napms(2000);
			clear_text(50);
			input='s';
			continue;
		}
	
		touchwin(title_win);
		touchwin(auth_win);
		touchwin(abs_win);
	
		if(chsub) {/*changes subject*/
			*sub_no= *sub_no+1;
			if(*sub_no>= nsubs){/*If it's past the last subject, it resets*/
				*sub_no=0;
			}
			chsub =0;
		}
		sub = subject_array[*sub_no];

		//Python script has loaded in subject file. Rename to save
		update_subs(*sub_no,subject_array ,nsubs);
		
		//Check that file is accessible (if not move on to next subject)
		acc =access(sub,F_OK); 
		
		if(!acc){/*python script get_entries won't copy if the entries have been read already*/
			refresh();
			rename(sub,temp);
		}
		else{
			refresh();
			chsub = 1;
			continue;
		}
		

		args->inp = &input;



		pthread_t uthreads[NTHREADS];
		rc = pthread_create(&uthreads[0], NULL, call_datprc,GE_SCRIPT);
		rc = pthread_create(&uthreads[1], NULL, navigator,(void* )args);
		
		for (ii=0; ii<NTHREADS; ii++){/*wait for threads to finish*/
			rc = pthread_join(uthreads[ii], NULL);
		}
	
		chsub = *(args->change_subject); 

	}while(input!='q');


	for(ii=0;ii<nsubs;ii++){
		free(subject_array[ii]);
	}

	FILE* f_res= fopen(RESUME_FILE,"w");
	fprintf(f_res,"%d %d", 0, *sub_no);
	fclose(f_res);

	endwin();	/* quit ncurses */

}

int main()
{
	int i, rc;
	bool colour;
	int subNo;
	if(!lineCounter(SUBJECTS_FILE)) {
		initscr();			
		if(COLS<80||LINES<20)
			bomb("Please use a larger terminal");
		col = 0; 
		keypad(stdscr, TRUE);		
		noecho();
		curs_set(0);
	}
	//In case subjects haven't been set yet
	do{
		move(1,30);
		printw("**Please choose at least one subject area**");
		refresh();
		move(1,30);
		refresh();
		move(1,20);
		if(s_menu()){
			bomb("Run program again to enact subject changes");
			endwin();
		}
	}while(!(lineCounter(SUBJECTS_FILE)));


	int re = resume(&sub_no);




	//initialise ncurses mode
	initscr();			
	if(COLS<MINCOLS||LINES<MINLINES)
		bomb("Terminal too small, please use a larger terminal");

	col = 0; /*col=0 is black and white mode*/
 	/*colours can be turned on by uncommenting*/
	//col = colours(); 

	//Keypad on, noecho to avoid mishaps and cursor off
	keypad(stdscr, TRUE);		
	noecho();
	curs_set(0);


	//If the system should not resume, run title screen and get entries in parallel
	if(!re){
		pthread_t threads[NTHREADS];
		rc = pthread_create(&threads[0], NULL, call_datprc,GE_SCRIPT);
		rc = pthread_create(&threads[1], NULL, title_screen,&col);
		
		/* wait for threads to finish*/
		for (i=0; i<NTHREADS; i++) 
		{
			rc = pthread_join(threads[i], NULL);
		}
	}
	else
		make_title(col); /*generate title otherwise*/


	add_colour(2,col);
	move(2,10);
	typewriter("v ",100);
	typewriter(VERSION,100);

	//run_arxinder runs the navigator simultaneously with get_entries. Current subject has been 
	//loaded by resume_script
	run_arxinder(col,&sub_no);

	char script[] = "py_scripts/mkchl.py";
	Py_Initialize();

	FILE* fp = fopen(script, "r");
	PyRun_SimpleFile(fp, script);

	Py_Finalize();
	fclose(fp);

	printf("Open chosen_entries.html with a browser application to view your selections.\n");
	return 0;
}

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/
