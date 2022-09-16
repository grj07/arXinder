 
/* 
                             ******************* 
******************************* C HEADER FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : arXinder                                                    **  
** filename  : menu.h                                                       ** 
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
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     DEFINITIONS AND MACROS                             **/ 
/**                                                                        **/ 
/****************************************************************************/ 
 
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
 
extern void clear_sublist();/*clears subjects (empties file)*/
extern void chosen_subjects(WINDOW *cho_win, int scl);/*displays subjects chosen from subjects.conf*/
extern char *make_menu(WINDOW *mwin,char *path_to_file,int item, int scl);
//prints an instant of the window to mwin, and returns name of for use in file path
extern char *refresh_menu(WINDOW *menu_win,char* path_to_file,
				int m_input, int *mm, int *sv);
//refresh_menu responds to minput and refreshes the new menu
extern bool s_menu();/*main subject menu, returns true if program needs to restart (possible changes to subjects)*/
/****************************************************************************/ 
/**                                                                        **/ 
/**                               EOF                                      **/ 
/**                                                                        **/ 
/****************************************************************************/ 

