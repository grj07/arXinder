/* 
                             ******************* 
******************************* C HEADER FILE ******************************* 
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
	      Main source code for arXinder.
*/ 
 
#define _TPL_C_SRC 

/****************************************************************************/ 
/**                                                                        **/ 
/**                     MODULES USED                                       **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#include <Python.h>
#include <ncurses.h> /*ncurses library, includes stdio.h*/
#include <math.h>    /*Floor and ceilings*/
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "gfuncs.h"
#include "pfuncs.h"
/****************************************************************************/ 
/**                                                                        **/ 
/**                     DEFINITIONS AND MACROS                             **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#define VERSION "0.1"
#define NTHREADS  2
#define MINCOLS 160
#define MINROWS 90
#define SUBJECTS_FILE "config/subjects.conf"
#define RESUME_FILE "state/res"
#define RESUME_SCRIPT "py_scripts/resume.py"
#define GE_SCRIPT "py_scripts/get_entries.py"
#define MAKE_HTML_SCRIPT "py_scripts/mk_html.py"
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
/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/
