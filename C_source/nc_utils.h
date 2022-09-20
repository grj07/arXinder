 
/* 
                             ******************* 
******************************* C HEADER FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : arXinder                                                    **  
** filename  : nc_utils.h                                                  ** 
** version   : 0.1                                                         ** 
** date      : 16 Sep 2022                                                 ** 
**                                                                         ** 
***************************************************************************** 
**                                                                         ** 
**   author  : Guy R. Jehu - grj90@pm.me                                   ** 
**                                                                         ** 
***************************************************************************** 
 
VERSION HISTORY: 
---------------- 
 
Version     : 0.1 
Date        : 16 Sep 2022 
Revised by  : Guy R. Jehu 
Description : Original version. 
 
*/ 
 
#ifndef _nc_utils_
#define _nc_utils_
/****************************************************************************/ 
/**                                                                        **/ 
/**                     MODULES USED                                       **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>
#include "utils.h"
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     DEFINITIONS AND MACROS                             **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#define GRAY 8  
#define GRAYR 500 /*colours scales will be terminal specific*/ 
#define GRAYG 500 /* so it is set off by default.*/
#define GRAYB 500

#define LRED 9
#define LREDR 1000
#define LREDG 800
#define LREDB 800

#define LYELLOW 10 
#define LYELLOWR 855
#define LYELLOWG 855
#define LYELLOWB 700

/****************************************************************************/ 
/**                                                                        **/ 
/**                     EXPORTED FUNCTIONS                                 **/ /**                                                                        **/ 
/****************************************************************************/ 
extern void bomb(char *msg);			        
/*exits with error message msg*/
extern int colours(); 
/*Colours checks if the terminal can do colours, and returns 1 if so*/
extern void addColour(int iCp,bool colours); 
/*sets colour ncp if qu true (!=0)*/
extern void removeColour(int iCp,bool colours);	
/*removes colour ncp if qu true (!=0)*/
extern void typewriter(char *typedText, int timeStep); 
/*Types out from cursor position with gap timestep in milliseconds*/
extern void clearText(int noOfChars);
/*erases no_ch from from cursor position*/
extern WINDOW *bTWin(char *title, int height, int width, int yPos,int xPos); 
/*generates bordered window with title*/
extern void updateWin(WINDOW *win,char **newContent, 
		int noOfLines,int scm, char *scrlInst);
/*Updates the contents (length no_of_lines multidimensional line array of 
 * a bordered window (win). scm is scroll value. 
 * scrlinst is string with scrolling instructions */
extern int newScrollValue(WINDOW *win,int noOfLines,int scrl); 
/*filter to stop scrolling past start/end of content*/
 
#endif
/****************************************************************************/ 
/**                                                                        **/ 
/**                               EOF                                      **/ 
/**                                                                        **/ 
/****************************************************************************/ 

