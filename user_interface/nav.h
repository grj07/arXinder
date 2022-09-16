 
/* 
                             ******************* 
******************************* C HEADER FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : arXinder                                                    **  
** filename  : navigator.c                                                 ** 
** version   : 0.1                                                         ** 
** date      : 16 Sep 2022                                                 ** 
**                                                                         ** 
***************************************************************************** 
**                                                                         ** 
**                                                                         ** 
***************************************************************************** 
 
VERSION HISTORY: 
---------------- 
 
Version     : 1 
Date        : 16 Sep 2022 
Revised by  : Guy R. Jehu 
Description : Original version.  This module contains definitions necessary 
				to extract data from files and navigate 
				displayed entries on the screen and display 
				metadata
*/ 
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     MODULES USED                                       **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <jansson.h>
#include "pfuncs.h"
#include "gfuncs.h"
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
 
struct _entries_header; /*structs for entry header from file - Read from json using jansson*/
struct _entry;          /*entries from file*/
struct _nav_arg_struct; /*needed to pass arguments to threaded function (navigator)*/

typedef struct _entries_header entries_header;
typedef struct _entry entry;
typedef struct _nav_arg_struct nav_arg_struct;

/****************************************************************************/ 
/**                                                                        **/ 
/**                     EXPORTED VARIABLES                                 **/ 
/**                                                                        **/ 
/****************************************************************************/ 
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     EXPORTED FUNCTIONS                                 **/ /**                                                                        **/ 
/****************************************************************************/ 
 
extern void update_subs(int n, char *sub_array[],int nsubs);/*updates the subject display*/
extern void ecount_update(int ent_count,int no_of_entries);/*updates the entry number displayed*/
/*manage_entries.c*/
extern entries_header *fetch_header(char *sub);/*loads header as json data through jansson*/
extern void set_header(entries_header *head);/*saves header to file*/
extern entry *fetch_entries(char *subject,int no_of_entries);/*populates array of entry structs with entry data*/
/*utils.c*/
extern void * navigator(void *arg_struct );/*browse arxiv entries*/
/****************************************************************************/ 
/**                                                                        **/ 
/**                               EOF                                      **/ 
/**                                                                        **/ 
/****************************************************************************/ 

