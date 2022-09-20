 
/* 
                             ******************* 
******************************* C SOURCE FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : arXinder                                                    **  
** filename  : nc_utils.c                                                  ** 
** version   : 0.1                                                         ** 
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
 
#include "nc_utils.h"
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     EXPORTED FUNCTIONS                                 **/ 
/**                                                                        **/ 
/****************************************************************************/ 
void bomb(char *msg){
	endwin();
	puts(msg);
	exit(1);
}

int colours(){
/*Colours checks if the terminal can do colours, and returns 1 if so*/
	int qu=0;
	if(!has_colors())
		bomb("Terminal cannot do colours!");
	else if(start_color()!=OK)
		bomb("Unable to start colours!");
	else if(!can_change_color())
		bomb("Can't change color");
	else{
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

void addColour(int iCp,bool colours){
	if(colours)
		attron(COLOR_PAIR(iCp));
}

void removeColour(int iCp,bool colours){
	if(colours)
		attroff(COLOR_PAIR(iCp));
}

void typewriter(char *typedText, int timeStep){
/*Types out from cursor position with gap timestep in milliseconds*/
	char *ch;
	ch = typedText;
	while(*ch){
		addch(*ch);
		ch++;
		refresh();
		usleep(timeStep*1000);
	}
}

void clearText(int noOfChars){
	int i;
	for(i=0;i<noOfChars;i++)
		addch(' ');
}

WINDOW *bTWin(char *title, int height, int width, int yPos,int xPos){	
	WINDOW *win;

	win = newwin(height,width,yPos,xPos);
	wbkgd(win,COLOR_PAIR(3));
	box(win, 0 , 0);		
	mvwaddstr(win,1,width/2-strlen(title)/2,title);
	return win;
}
 
void updateWin(WINDOW *win,char **newContent, 
		int noOfLines,int scrl,char *scrlInst){

	WINDOW *cWin; /*content window*/
	int height, width;
	int instLength = strlen(scrlInst);
	int i, iMax;

	getmaxyx(win,height,width); 	/*measures size of window*/
	iMax = min(height-3,noOfLines); /*last line printed to window*/

	if(noOfLines>height-3){
	/*case where too much content for window - 
	 * display scrolling instructions*/
		mvwaddstr(win , 1, width- instLength-1,scrlInst);
	}
	else{
		wmove(win,1, width- instLength-1);
		for(i=0;i<instLength;i++){
			waddch(win,' ');
		}
	}

	cWin = derwin(win,height-3,width-4,2,2);
	wbkgd(cWin,COLOR_PAIR(4));
	wclear(cWin);

	for(i=scrl;i<iMax+scrl;i++){
	/*print the lines which fit in content window*/
		mvwaddstr(cWin,i-scrl,0,newContent[i]);
	}
	wattroff(cWin,A_BOLD);
	wrefresh(win);
	wrefresh(cWin);
}

int newScrollValue(WINDOW *win,int noOfLines,int scrl){
	int height, nScrl;

	height = getmaxy(win);

	nScrl = min(noOfLines - (height-3),scrl);
	nScrl = max(0,nScrl);
	return(nScrl);
}

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

