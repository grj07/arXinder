 
/* 
                             ******************* 
******************************* C SOURCE FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : arXinder                                                    **  
** filename  : utils.c                                                     ** 
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
 
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     MODULES USED                                       **/ 
/**                                                                        **/ 
/****************************************************************************/ 
#include "utils.h" 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     EXPORTED VARIABLES                                 **/ 
/**                                                                        **/ 
/****************************************************************************/ 
 
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

int max(int a,int b){
	if(a>b)
		return(a);
	else
		return(b);
}

int min(int a,int b){
	if(a<b)
		return(a);
	else
		return(b);
}
 
int lineCounter(char *filePath){
	FILE *f = fopen(filePath, "r"); 
	if(f==NULL)
		return(0);
	int totalLines=0;
	char c;
	for(c=getc(f);c!=EOF;c=getc(f))
        	if (c == '\n') 
            		totalLines++;
	fclose(f);
	return(totalLines);
}

char **toLines(char *content, int lineWidth , int *noOfLines){
	int i=0, il =1, iMax =strlen(content);
	int *nlPos= malloc(sizeof(int)*iMax); 
	/*an array recording the position of new lines.*/
	int iPrevNewLine = 0;
	int iPrevSpace = 0;

	nlPos[0]=-1; 				
	for(i = 0;i<iMax;i++){
	/*populates array with the final space 
	 * character that fits on the line*/
		if((char) content[i]==' '){ 
		/*identifies space character, checks if it's past width*/
			if(i-iPrevNewLine>=lineWidth){
				nlPos[il] = iPrevSpace; 
				/*if so, the previous space ends the line*/
				iPrevNewLine = nlPos[il]; 
				il++;
			}
			iPrevSpace = i;
		}
	}	
	if(i-iPrevNewLine>=lineWidth){
		/*catch the last line in case needed (could be mid word)*/
		nlPos[il] = iPrevSpace; 
		il++;
	}
	/*il gives number of lines, nlPos[i<il] gives end of line positions*/ 
	*noOfLines = il; 
	/*function assigns the number of lines to the pointer*/
	nlPos[*noOfLines]=iMax; /*an extra new line at the end of the content*/


	//Now to populate the appropriate pointers with the lines
	size_t lineSize = (lineWidth+1)*sizeof(char);
	char *ptr = calloc((*noOfLines)*lineSize,sizeof(char));
	/*Allocate whole array*/
	char **ptrArray = malloc(sizeof(char *)*(*noOfLines));
	/*Allocate memory for the pointers at the start of the lines*/

	for(il = 0;il<*noOfLines;il++){
	/*loop through lines*/
		for(i=0;i<nlPos[il+1]-nlPos[il]-1;i++){ 
		/*copies line to row*/
			*(ptr+i+il*lineSize) = content[i+nlPos[il]+1];
		}
		ptr[il*lineSize+nlPos[lc+1]-nlPos[lc]]='\0';
		*(ptrArray+il) = ptr+il*lineSize;
	}
	free(nlPos);
	return(ptrArray);
}

char *joinStrings(char *str1,char *str2){
	char *str;
	size_t strSize, str1Size;
	int i;

	str1Size = strlen(string_1);
	strSize = str1Size+strlen(str2);

	str = (char *)malloc((strSize+1)*sizeof(char));

	for(i=0;i<str1Size;i++){
		*(str + i)= *(str1+i);
	}
	for(i=str1Size;i<strSize;i++){
		*(str + i)= *(str2+i-str1Size);
	}
	*(str+strSize)='\0';

	return(str);
}

char **commaSplit(char *str, int *height, int width){


	char **ptrArray, *ptr;
	char ch;
	int i = 1, ic=1; /*ic counts number of strings between commas*/
	char *buf=malloc(sizeof(char)*(strlen(str)+1));

	const char delim[2] = ",";
	char *tok = NULL;
	int strLength;	

	strcpy(buf,str);
 	ch = buf[0]
	buf[strlen(str)]='\0';

	while(ch){			/*count the number of commas to */
		ch = buf[i];			/*determine the height*/
		if(ch == ',')
			ic++;
		i++;
	}

	ptrArray = malloc(sizeof(char *)*ic) ;
	ptr = calloc(ic*width,sizeof(char));

	tok = strtok(buf,delim);

	for(i=0;i<ic;i++){
		strLength = min(strlen(tok),width);
		strncpy(ptr+i*width,tok,strLength);
		*(ptr+i*(width)+strLength)='\0';
		*(ptrArray+i) = ptr+i*(width);
		tok = strtok(NULL,delim);
	}

	*height = ic;
	free(authBuf);
	return(ptrArray);
}

void free2DArray(char **ptrArray) {
	free(*ptrArray);
	free(ptrArray);
}

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

