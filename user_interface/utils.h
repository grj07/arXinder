/* 
                             ******************* 
******************************* C HEADER FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : arXinder                                                    **  
** filename  : TPL.H                                                       ** 
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
Description : Original version. This module contains simple functions generic
				in C programming, such as exiting a 
				program with a message, comparing two numbers,
			       	and basic string manipulation and file 
				management.
 
*/ 
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     MODULES USED                                       **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#include <stdio>
#include <stdlib.h>
#include <string.h>
 
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
 
extern void bomb(char *msg);			        
/*exits with error message msg*/
extern int min(int a,int b);
/*Maximum of two integers*/
extern int max(int a,int b);		
/*Minimum of two integers*/
extern int lineCounter(char *filePath);	
/*counts the number of lines in file*/
extern char *joinStrings(char *string_1, char *string_2);
/*Concatenates string1 and string2*/
extern char **toLines(char *content, int width,int* height); 
/*Returns pointer to array of length no_of_lines with no_of_lines rows
 * of strings of width line_width.*/
extern void free2DArray(char **lines_buf);			
/*free multidim array*/
extern char **commaSplit(char *auth_str, int* height, int width);
/*splits a string separated by commas into pointer to array of height 
 * strings of length witdth*/
/****************************************************************************/ 
/**                                                                        **/ 
/**                               EOF                                      **/ 
/**                                                                        **/ 
/****************************************************************************/ 

