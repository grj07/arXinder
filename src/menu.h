 
/* 
                             ******************* 
******************************* C HEADER FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : arXinder                                                    **  
** filename  : menu.h                                                      ** 
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
Description : Original version.  Subjects can be selected through the menu 
				defined	here, which is modifies subjects.conf.
*/ 
 
#ifndef _menu_
#define _menu_
/****************************************************************************/ 
/**                                                                        **/ 
/**                     MODULES USED                                       **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#include "utils.h"
#include "nc_utils.h"
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     DEFINITIONS AND MACROS                             **/ 
/**                                                                        **/ 
/****************************************************************************/ 

#define SUBJECT_FILE "config/subjects.conf"
#define CAT_FILE "state/cat_list"
#define SUBJ_PATH "state/subjects/"

#define C_INSTRUCTIONS "c - clear subjects"
#define Q_INSTRUCTIONS "q - quit menu "
/****************************************************************************/ 
/**                                                                        **/ 
/**                     TYPEDEFS AND STRUCTURES                            **/ 
/**                                                                        **/ 
/****************************************************************************/ 
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     EXPORTED VARIABLES                                 **/ 
/**                                                                        **/ 
/****************************************************************************/ 
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     EXPORTED FUNCTIONS                                 **/ /**                                                                        **/ 
/****************************************************************************/ 
 
extern void clearSubjects();
/*clears subjects (empties file)*/
extern void chosenSubjects(WINDOW* choicesWin, int scrl);
/*displays subjects chosen from subjects.conf*/
extern char *makeMenu(WINDOW* mWin,char* filePath,int item, int scrl);
/*prints an instance of the menu to mWin, *
 *and returns name of for use in file path */
extern char *refreshMenu(WINDOW* mWin,char* filePath,int input, 
						int* menuItem,int* scrl);
/*Refreshes menu after key pressed (input)*/
extern bool subjectMenu();
/*main subject menu, returns true if program needs to restart*/

#endif 
/****************************************************************************/ 
/**                                                                        **/ 
/**                               EOF                                      **/ 
/**                                                                        **/ 
/****************************************************************************/ 

