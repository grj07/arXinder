 
/* 
                             ******************* 
******************************* C SOURCE FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : arXinder                                            **  
** filename  : menu.c                                                       ** 
** version   : 0.1                                                           ** 
** date      : 16 Sep 2022                                                 ** 
**                                                                         ** 
***************************************************************************** 
**                                                                         ** 
** author    : Guy R. Jehu                                                 ** 
**                                                                         ** 
***************************************************************************** 
 
VERSION HISTORY: 
---------------- 
 
Version     : 0.1 
Date        : 16 Sep 2022 
Revised by  : Guy R. Jehu 
Description : Original version. 
 
*/ 
 
#include "menu.h" 
void clearSubjects(){
/*clears subjects (empties file)*/
	FILE *subFile;
	subFile = fopen(SUBJECT_FILE,"w");
	fclose(subFile);
}

 
void chosenSubjects(WINDOW* choicesWin,int scrl){
/*displays subjects chosen from subjects.conf*/
	FILE *subFile;
	int i=0;
	int wh, ww; /*height and width of the window*/
	char *buf=NULL;
	size_t bufSize; 
	ssize_t lineSize;

	getmaxyx(choicesWin,wh,ww);

	char *menuItem = calloc(ww-1,sizeof(char));

	subFile = fopen(SUBJECT_FILE,"r");
	
	lineSize = getline(&buf,&bufSize,subFile);

	while(lineSize >=0){
		strcpy(menuItem,buf);
		*(menuItem+strlen(buf)-1)='\0';

	      	wmove(choicesWin,i-scrl,0);
		wclrtoeol(choicesWin);

		if(i>=scrl)
			mvwaddstr(choicesWin,i-scrl,1,menuItem);

		wattroff(choicesWin,A_REVERSE);
		lineSize = getline(&buf,&bufSize,subFile);

		if(i>wh+scrl)
			break;

		i++;
	}
	fclose(subFile);
	free(buf);
	free(menuItem);
}

char *makeMenu(WINDOW *mWin,char *filePath,int item, int scrl){
/*prints an instant of the window to mwin, and returns ptr 
 *to string giving the subject selected*/

	FILE *menuFile; /*menu list stored in individual files*/
	int i=0;
	int wh, ww;
	char *buf=NULL;
	char *selectedItem = NULL;
	size_t bufSize; 
	ssize_t lineSize;

	getmaxyx(mWin,wh,ww);
	size_t menuItemSize=(ww-1)*sizeof(char);
	char *menuItem = calloc(ww-1,sizeof(char));

	menuFile = fopen(filePath,"r");

	lineSize = getline(&buf,&bufSize,menuFile);
	while(lineSize >=0){
		strncpy(menuItem,buf,ww-1);
		*(menuItem+min(strlen(buf)-1,ww-1))='\0';
		/*newline character removed*/

	      	wmove(mWin,i-scrl,0);/*clear old menu item in this spot*/
		wclrtoeol(mWin);
		
		if(i==item){
		/*highlight item selected, and copy for output*/
			selectedItem = (char *)malloc(menuItemSize);
			strcpy(selectedItem,menuItem);
			wattron(mWin,A_REVERSE);
		}

		/*only print the menu items that can fit in window*/
		if(i>=scrl)
			mvwaddstr(mWin,i-scrl,1,menuItem);

		wattroff(mWin,A_REVERSE);
		lineSize = getline(&buf,&bufSize,menuFile);

		if(i>wh+scrl)/*stop at the bottom of window*/
			break;

		i++;
	}
	fclose(menuFile);

	free(buf);
	free(menuItem);
	return(selectedItem); /*memory must be freed in main*/
}

char *refreshMenu(WINDOW *mWin,char* filePath,int input,int* menuItem, 
								int* scrl){
/*responds to input and refreshes the new menu*/
	int wh, lineCount; 
	/*window dims and number of lines for menu*/
	char *categ; /*category selected*/

	lineCount = lineCounter(filePath);
	wh = getmaxy(mWin);

	switch(input){
     	/* adjusts menu according to item selected and scroll value*/
		case KEY_DOWN:
			*menuItem = *menuItem+1;
			if(*menuItem>=lineCount){
				*menuItem=0;
				*scrl=0;
			}
			if(lineCount<wh)
				*scrl=0;
			else if(*menuItem>=wh-*scrl)
				*scrl = min(*scrl+1,lineCount-wh);
			break;
		case KEY_UP:
			*menuItem = *menuItem-1;
			if(*menuItem<0){
				*menuItem=lineCount-1;
				*scrl=lineCount-wh;
			}
			if(lineCount<wh)
				*scrl=0;
			else if(*scrl>0 && *menuItem<*scrl)
				*scrl = *scrl-1;
			break;
		case '\n':
			break;
	}

	categ =makeMenu(mWin, filePath, *menuItem,*scrl);	
	return(categ); /*memory freed in main*/
}

bool subjectMenu() {
/*subject menu - returns true if program needs to restart))*/
	FILE *subFile;
	/*subject lists extracted from files*/

	WINDOW *mWin, *catWin, *subWin, *choicesWin; 

	int cItem = 0,cScrl = 0, sItem = 0, sScrl = 0;
	/*scroll values for category and subject menus*/

	int input;

	bool changesMade=0;
	/*if changes made to subjects program must be restarted*/

	char *catStr, *sbjStr;
	char *filePath = malloc(60*sizeof(char));

	mWin=bTWin("Subject Settings",LINES-5,COLS-4,5,2);
	wbkgd(mWin,COLOR_PAIR(2));
	mvwaddstr(mWin,1,COLS-25,C_INSTRUCTIONS);
	wrefresh(mWin);
	wbkgd(mWin,COLOR_PAIR(4));

	catWin = derwin(mWin,9,45,2,1); /*categories*/
	subWin = derwin(mWin,LINES-8,COLS-60,2,48);/*subjects*/
	choicesWin = derwin(mWin,LINES-20,45,14,1);/*chosen subjects*/

	chosenSubjects(choicesWin,0);
		
	wrefresh(catWin);
	wrefresh(choicesWin);

	catStr = makeMenu(catWin,CAT_FILE,cItem,cScrl);
	wrefresh(catWin);

	input = getch();

	while(input!='q'){ 
	/*choose category*/
		free(catStr);
		catStr = refreshMenu(catWin,CAT_FILE,input,&cItem,&cScrl);

		if(input == 'c'){
			clearSubjects();
			wclear(choicesWin);
			chosenSubjects(choicesWin,0);
			wrefresh(choicesWin);
			changesMade = 1;
		}

		if(input=='\n'){
		/*category selected, choose subject*/
			free(filePath);
			filePath = joinStrings(SUBJ_PATH,catStr);

			sbjStr = makeMenu(subWin,filePath,sItem,sScrl);
			wrefresh(subWin);

			input = getch();

			while(input!='q'){
				if(input == 'c'){
					clearSubjects();
					wclear(choicesWin);
					chosenSubjects(choicesWin,0);
					wrefresh(choicesWin);
				}
				if(input == 'c')
					clearSubjects();
				if(input == '\n'){
				/*subject selected, add to list*/

					subFile=fopen(SUBJECT_FILE,"a");
					fprintf(subFile,"%s\n",
							strtok(sbjStr ," ,"));
					fclose(subFile);
					chosenSubjects(choicesWin,0);
					wrefresh(choicesWin);
					changesMade = 1;
				}
				
				free(sbjStr);
				sbjStr = refreshMenu(subWin,filePath,
							input,&sItem,&sScrl);
				wrefresh(subWin);
				input = getch();
			}
			free(sbjStr);
		}

		sItem = 0;
		sScrl = 0;
		wclear(subWin);
		wrefresh(subWin);
		wrefresh(catWin);
		input = getch();
	}
	wclear(mWin);
	wrefresh(mWin);
	delwin(mWin);
	free(catStr);
	free(filePath);
	move(0,5);
	clearText(40);
	refresh();
	return(changesMade);
}
/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

