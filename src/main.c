/* 
                             ******************* 
******************************* C SOURCE FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : ArXinder                                                    **  
** filename  : main.c                                                      ** 
** version   : 0.1                                                         ** 
** date      : 16 Sep 2022             		                           ** 
**                                                                         ** 
***************************************************************************** 
**                                                                         ** 
**    author : Guy R. Jehu - grj90@pm.me                                   ** 
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
#include "main.h"

/****************************************************************************/
/**                                                                        **/
/**                     LOCAL FUNCTIONS                                    **/
/**                                                                        **/
/****************************************************************************/
void* runPyScript(void* scriptPath){
	FILE *fp = fopen(scriptPath, "r");

	PyRun_SimpleFile(fp, scriptPath);

	fclose(fp);

	return NULL;
}

//Title screen: animation while first subject entries load
void* titleScreen(void* col){
	bool colours = *(int*)col;
	char title1[] = "ar", title2[]= "inder" ;
	char subtitle[]="sort and file the new papers from the arXiv.";
	int disp = (strlen(title1)+strlen(title2)+1)/2;
	int i;

	curs_set(0);
	move(LINES/2,COLS/2-disp);
	addColour(1,colours);
	typewriter(title1,120);
	refresh();
	addColour(2,colours);
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
	addColour(1,colours);
	typewriter(title2,120);
	
	//Type out subtitle
	usleep(500000);
	addColour(3,colours);
	move(LINES/2+2,COLS/2-strlen(subtitle)/2);
	typewriter(subtitle,30);
	move(1,1);

	for(i=1;i<=LINES/2-2;i++){
		deleteln();
		refresh();
		usleep(20000);
	}

	for(i=1;i<=COLS/2-disp-1;i++){
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
	return NULL;
}

void makeTitle(bool colours){
	char title[]="arXinder";
	char subtitle[]="new papers from the arXiv:";

	addColour(1,colours);
	mvaddstr(2,0,title);
	addColour(2,colours);
	mvaddch(2,2,'X');
	mvaddch(1,1,'\\');
	mvaddch(3,3,'\\');
	mvaddch(1,3,'/');
	mvaddch(3,1,'/');

	addColour(3,colours);
	mvaddstr(4,0,subtitle);

	curs_set(0);
	refresh();
	noecho();
}

int resume(int*  subNo){
	FILE *confFile;
	FILE *scriptFile;
	int res=0;

	scriptFile = fopen(RESUME_SCRIPT, "r");

	PyRun_SimpleFile(scriptFile,RESUME_SCRIPT);

	fclose(scriptFile);

	confFile = fopen(RESUME_FILE,"r");

	fscanf(confFile,"%d %d", &res, subNo);

	fclose(confFile);
	return(res);
}

void runArXinder(int colours, int* subNo){
	WINDOW *titleWin;
       	WINDOW *authWin; 
	WINDOW *absWin; 

	mvprintw(4,COLS-strlen(SUB_INST),SUB_INST);
	mvprintw(3,COLS-strlen(SORT_INST),SORT_INST);
	mvprintw(1,COLS-strlen(QUIT_INST),QUIT_INST);
	removeColour(2,colours);
	refresh();

	int input;
	int i = 0;

	char* sub; 	

	int noOfSubjects = lineCounter(SUBJECTS_FILE);
	char* subjectArray[noOfSubjects];
	char* lineBuf=NULL;
	size_t lineBufSize;

	FILE* subFile;
	FILE* resFile;

	bool chSub=0;
	bool acc = 0;

	//entries to be displayed here
	titleWin = bTWin("Title",8,COLS/2-2,5,2);
	authWin = bTWin("Authors",8,COLS/2-2,5,COLS/2+1);
	absWin = bTWin("Abstract", LINES-13,COLS-4,13,2);

	wrefresh(titleWin);
	wrefresh(authWin);
	wrefresh(absWin);

	
	//Subjects in an array to be loaded into array of strings
	subFile= fopen(SUBJECTS_FILE,"r");

	for(i=0;i<noOfSubjects;i++){
		getline(&lineBuf,&lineBufSize,subFile);
		subjectArray[i] = malloc((strlen(lineBuf))*sizeof(char));
		strncpy(subjectArray[i],lineBuf,strlen(lineBuf)-1);
		*(subjectArray[i]+strlen(lineBuf)-1)='\0';
	}

	free(lineBuf);
	fclose(subFile);



	refresh();
	flushinp(); 
	/*Clears the keyboard buffer*/

	input = '0';
	nav_arg_struct* args = malloc(sizeof(nav_arg_struct));
	args->tWin = titleWin;
	args->aWin = authWin;
	args->abWin = absWin;
	args->inp = &input;
	args->changeSubject = &chSub;

	chSub = 0;
	do{
		if(input == 's'){
			if(subjectMenu()) 	
				bomb(RESTART_INST);
		}
		//Need to reload subjects if they change.
		if(!lineCounter(SUBJECTS_FILE)) {
			move(1,30);
			printw("**Please choose at least one subject area**");
			refresh();
			move(1,30);
			napms(2000);
			clearText(50);
			input='s';
			continue;
		}
	
		touchwin(titleWin);
		touchwin(authWin);
		touchwin(absWin);
	
		if(chSub) {/*changes subject*/
			*subNo= *subNo+1;
			if(*subNo>= noOfSubjects)
				*subNo=0;
			chSub =0;
		}
		sub = subjectArray[*subNo];

		updateSubs(*subNo, subjectArray, noOfSubjects);
		
		acc =access(sub,F_OK); 
		/*access returns 0 if the file can be accessed*/
		
		if(!acc){
		/*rename subject file to cursub*/
			rename(sub,CUR_SUB_FILE);
		}
		else{
		/*change subject and skip otherwise*/
			chSub = 1;
			continue;
		}
		

		args->inp = &input;



		pthread_t uthreads[NTHREADS];
		pthread_create(&uthreads[0], NULL, runPyScript,GE_SCRIPT);
		pthread_create(&uthreads[1], NULL, navigator,(void* )args);
		
		for (i=0; i<NTHREADS; i++){/*wait for threads to finish*/
			pthread_join(uthreads[i], NULL);
		}
	
		chSub = *(args->changeSubject); 

	}while(input!='q');


	free(args);

	for(i=0;i<noOfSubjects;i++){
		free(subjectArray[i]);
	}

	resFile= fopen(RESUME_FILE,"w");
	fprintf(resFile,"%d %d", 0, *subNo);
	fclose(resFile);

	endwin();	/* quit ncurses */

}

/****************************************************************************/
/**                                                                        **/
/**                     MAIN FUNCTION                                      **/
/**                                                                        **/
/****************************************************************************/ 
int main(void){
	int i, re;
	bool colour = 0;
	int subNo;

	FILE *fp;

	if(!lineCounter(SUBJECTS_FILE)) {
		initscr();			
		if(COLS<80||LINES<20)
			bomb("Please use a larger terminal");
		colour = 0; 
		keypad(stdscr, TRUE);		
		noecho();
		curs_set(0);
	//In case subjects haven't been set yet
		do{
			move(1,30);
			printw("**Please choose at least one subject area**");
			refresh();
			move(1,30);
			refresh();
			move(1,20);
			if(subjectMenu()){
				bomb("Run program again to enact subject changes");
				endwin();
			}
		}while(!(lineCounter(SUBJECTS_FILE)));

	}

	Py_Initialize(); /*run python scripts in C*/

	re = resume(&subNo);
	/*True if system should resume (no new feeds)*/


	//initialise ncurses mode
	initscr();			
	if(COLS<MIN_COLS||LINES<MIN_LINES)
		bomb("Terminal too small, please use a larger terminal");

 	/*colours can be turned on by uncommenting*/
	//colour = colours(); 


	//Keypad on, noecho to avoid mishaps and cursor off
	keypad(stdscr, TRUE);		
	noecho();
	curs_set(0);


	if(!re){
	/*run title screen and get entries in parallel*/
		pthread_t threads[NTHREADS];
		pthread_create(&threads[0], NULL, runPyScript, GE_SCRIPT);
		pthread_create(&threads[1], NULL, titleScreen, &colour);
		
		/* wait for threads to finish*/
		for (i=0; i<NTHREADS; i++){
			pthread_join(threads[i], NULL);
		}
	}
	else
		makeTitle(colour); /*generate title otherwise*/


	addColour(2,colour);
	move(2,10);
	typewriter("v ",100);
	typewriter(VERSION,100);

	
	runArXinder(colour,&subNo);


	fp = fopen(MAKE_HTML_SCRIPT, "r");

	PyRun_SimpleFile(fp, MAKE_HTML_SCRIPT);

	fclose(fp);

	Py_Finalize();
	printf(VIEW_CH_INST);
	return 0;
}

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/
