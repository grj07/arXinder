 
/* 
                             ******************* 
******************************* C HEADER FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : arXinder                                                    **  
** filename  : nav.h                                                 ** 
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
Description : Original version. Functions and structs  necessary to extract 
				data from files and navigate displayed 
				entries on the screen and display metadata
*/ 
 
#ifndef _nav_
#define _nav_
/****************************************************************************/ 
/**                                                                        **/ 
/**                     MODULES USED                                       **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#include "utils.h"
#include "nc_utils.h"
#include <unistd.h>
#include <jansson.h>
/****************************************************************************/ 
/**                                                                        **/ 
/**                     DEFINITIONS AND MACROS                             **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#define PREV_FEEDS "state/p_feeds/"
#define CUR_SUBJECT "state/cursub"
#define SAVED_ENTRIES "state/saved_entries.d"
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     TYPEDEFS AND STRUCTURES                            **/ 
/**                                                                        **/ 
/****************************************************************************/ 
 
struct _entries_header; 
/*structs for entry header from file - Read from json using jansson*/
struct _entry;          /*entries from file*/
struct _nav_arg_struct; /*needed to pass arguments to threaded function 
			  (navigator)*/

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
 
extern void updateSubs(int n, char *subArray[],int nSubs);
/*updates the subject display*/
extern void updateEntryCount(int entryCount,int noOfEntries);
/*updates the entry number displayed*/
extern entries_header *fetchHeader(char *filePath);
/*loads header as json data through jansson*/
extern void setHeader(entries_header *head);/*saves header to file*/
extern entry *fetchEntries(char *filePath,int noOfEntries);
/*populates array of entry structs with entry data*/
extern void * navigator(void *argStruct );
/*main function for browsing arxiv entries*/
#endif
/****************************************************************************/ 
/**                                                                        **/ 
/**                               EOF                                      **/ 
/**                                                                        **/ 
/****************************************************************************/ 

