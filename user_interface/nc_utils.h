 
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
**                                                                         ** 
***************************************************************************** 
 
VERSION HISTORY: 
---------------- 
 
Version     : 0.1 
Date        : 16 Sep 2022 
Revised by  : Guy R. Jehu 
Description : Original version. 
 
*/ 
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     MODULES USED                                       **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#include <ncurses.h>
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     DEFINITIONS AND MACROS                             **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#define GRAY 8  
#define GRAYR 500 /*colours scales will be terminal specific, so it is set off 
#define GRAYG 500  *by default.*/
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
extern void add_colour(int ncp,int qu); 
/*sets colour ncp if qu true (!=0)*/
extern void rem_colour(int ncp,int qu);	
/*removes colour ncp if qu true (!=0)*/
extern void clear_text(int no_ch);
/*erases no_ch from from cursor position*/
extern WINDOW *bt_win(char *title, int height, int width, int ypos,int xpos); 
/*Bordered window to be filled with content*/
extern void update_win(WINDOW *win,char **new_content, 
		int no_of_lines,int scm, char * scrl_inst);
/*Updates the contents (length no_of_lines multidimensional line array of 
 * a bordered window (win). scm is scroll value. 
 * scrlinst is string with scrolling instructions */
extern int new_scroll_value(WINDOW *win,int tot_lines,int scv); 
/*filter to stop scrolling start/end of content*/
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                               EOF                                      **/ 
/**                                                                        **/ 
/****************************************************************************/ 

