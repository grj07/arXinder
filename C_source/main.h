/* 
                             ******************* 
******************************* C HEADER FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : ArXinder                                                    **  
** filename  : main.c                                                      ** 
** version   : 0.1                                                         ** 
** date      : 16 Sep 2022             		                           ** 
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
Description : Original version.
	      Main source code for arXinder.
*/ 
 
#ifndef _main_
#define _main_

/****************************************************************************/ 
/**                                                                        **/ 
/**                     MODULES USED                                       **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#include "menu.h"
#include "nav.h"
#include <Python.h>
#include <pthread.h>
#include <time.h>

/****************************************************************************/ 
/**                                                                        **/ 
/**                     DEFINITIONS AND MACROS                             **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#define VERSION "0.1"
#define NTHREADS  2
#define MIN_COLS 50
#define MIN_LINES 20
#define SUBJECTS_FILE "config/subjects.conf"
#define RESUME_FILE "state/res"
#define RESUME_SCRIPT "py_scripts/resume.py"
#define GE_SCRIPT "py_scripts/get_entries.py"
#define MAKE_HTML_SCRIPT "py_scripts/mk_html.py"
#define CUR_SUB_FILE "state/cursub"

#define VIEW_CH_INST "Open chosen_entries.html with a browser application to view your selections.\n"
#define SUB_INST "s - change subjects"
#define SORT_INST "Use backspace to reject, return to save, u to undo"
#define QUIT_INST "press q to quit"
#define RESTART_INST "Please restart program to enact subject changes"
/****************************************************************************/ 
/**                                                                        **/ 
/**                     TYPEDEFS AND STRUCTURES                            **/ 
/**                                                                        **/ 
/****************************************************************************/ 
struct _nav_arg_struct{ /*struct for argument of navigator*/
	WINDOW *tWin;
       	WINDOW *aWin;
       	WINDOW	*abWin;
	int *inp;
	bool *changeSubject;
};
#endif
/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/
