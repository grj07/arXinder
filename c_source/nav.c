 
/* 
                             ******************* 
******************************* C SOURCE FILE ******************************* 
**                           *******************                           ** 
**                                                                         ** 
** project   : arXinder                                                    **  
** filename  : nav.c                                                       ** 
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
 
#include "nav.h"
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     TYPEDEFS AND STRUCTURES                            **/ 
/**                                                                        **/ 
/****************************************************************************/ 
struct _entries_header{
	char subject[20];
	int rawNoOfEntries;
	char date[9];
	char dateF[18];
	int curEntryNo;
	int filteredNoOfEntries;
};

struct _entry
{
	char arxivNo[12];
	char title[300];
	char authors[3000];
	char abstract[2000];
};

struct _nav_arg_struct{ /*struct for argument of navigator*/
	WINDOW *tWin;
       	WINDOW *aWin;
       	WINDOW	*abWin;
	int *inp;
	bool *changeSubject;
};
 
/****************************************************************************/ 
/**                                                                        **/ 
/**                     EXPORTED FUNCTIONS                                 **/ 
/**                                                                        **/ 
/****************************************************************************/ 
void updateSubs(int subjectNo,char *subjectArray[],int noOfSubjects){
/*updates the subject display*/
	int i, iMod;
	
	int xPos = strlen(subjectArray[subjectNo])+5;

	for(i= subjectNo;i<noOfSubjects+subjectNo;i++){
	/*computes how many characters needed to display subjects*/
		iMod = i % noOfSubjects;
		xPos = xPos + max(strlen(subjectArray[iMod])+5,0);
	}

	move(2,20); /*print the current subject in boldface*/
	clearText(xPos);
	attron(A_BOLD);
	mvprintw(2,20,subjectArray[subjectNo]);
	attroff(A_BOLD);

	xPos = strlen(subjectArray[subjectNo])+5;

	for(i=subjectNo+1;i<noOfSubjects+subjectNo;i++){
	/*prints subject array across top of screen*/
		iMod = i % noOfSubjects;
		mvprintw(2,20+xPos,subjectArray[iMod]);
		xPos = xPos + max(strlen(subjectArray[iMod])+5,0);
	}
}

void updateEntryCount(int entryNo,int noOfEntries){
/*updates the entry number displayed*/
	move(3,19);
	clearText(9);
	if(entryNo<10)
		mvprintw(3,21,"%d/%d",entryNo,noOfEntries);
	else if(entryNo < 100)
		mvprintw(3,20,"%d/%d",entryNo,noOfEntries);
	else
		mvprintw(3,19,"%d/%d",entryNo,noOfEntries);
}


entries_header *fetchHeader(char *filePath){ 
/*loads header as json data through jansson*/
	entries_header *head = malloc(sizeof(entries_header));	
	json_t* jTemp = NULL;
	
	char * lineBuf=NULL;
	size_t lineSize;
	json_error_t error;
	FILE *fp;
	
	fp = fopen(filePath,"r");

	getline(&lineBuf, &lineSize, fp);
	json_t *root = json_loads(lineBuf,0,&error);
	jTemp = json_object_get(root, "subject");
	strcpy(head->subject,(char*) json_string_value(jTemp));
	jTemp = json_object_get(root, "raw_no_of_entries");
	head->rawNoOfEntries = (int) json_integer_value(jTemp);
	jTemp = json_object_get(root, "date");
	strcpy(head->date,(char*) json_string_value(jTemp));
	jTemp = json_object_get(root, "datef");
	strcpy(head->dateF,(char*) json_string_value(jTemp));
	jTemp = json_object_get(root, "cur_entry_no");
	head->curEntryNo = (int) json_integer_value(jTemp);
	jTemp = json_object_get(root, "filtered_no_of_entries");
	head->filteredNoOfEntries = (int) json_integer_value(jTemp);

	fclose(fp);
	free(jTemp);
	free(lineBuf);
	return(head);
}

void setHeader(entries_header *head){ /*saves header to file*/
	char *str, *filePath;
	json_t* jTemp = NULL;
	char* newHead = NULL;/*string version of new header*/
	
	char * lineBuf=NULL;
	size_t lineSize;
	ssize_t lineLength;
	FILE *rFile;
	FILE *wFile;
	

	/*create json object from struct*/
	json_t* root = json_object(); 
	jTemp = json_string(head->subject);
	json_object_set_new(root, "subject", jTemp);
	jTemp = json_integer(head->rawNoOfEntries);
	json_object_set_new(root, "raw_no_of_entries", jTemp);
	jTemp = json_string(head->date);
	json_object_set_new(root, "date", jTemp);
	jTemp = json_string(head->dateF);
	json_object_set_new(root, "datef", jTemp);
	jTemp = json_integer(head->curEntryNo);
	json_object_set_new(root, "cur_entry_no", jTemp);
	jTemp = json_integer(head->filteredNoOfEntries);
	json_object_set_new(root, "filtered_no_of_entries", jTemp);
	newHead = json_dumps(root,0);

	str = joinStrings(PREV_FEEDS,head->subject);
	filePath = joinStrings(str,head->date);

	wFile = fopen(filePath,"w");
	fputs(newHead,wFile);
	fputs("\n",wFile);

	rFile = fopen(CUR_SUBJECT,"r");
	lineLength = getline(&lineBuf, &lineSize, rFile);
	lineLength = getline(&lineBuf, &lineSize, rFile);

	while(lineLength>= 0){
	/*copying rest of current file to p_feed file*/
		fputs(lineBuf,wFile);
		lineLength = getline(&lineBuf, &lineSize, rFile);
	}

	fclose(rFile);
	fclose(wFile);

	json_decref(root);/*valgring complains abour decref on ints*/

	free(str);
	free(filePath);
	free(newHead);
	free(lineBuf);
}

entry *fetchEntries(char *subject,int noOfEntries){
/*populates array of entry structs with entry data*/
	int i, sw;
	int entryNo = 0;
	int lineCount = 5*noOfEntries+1;

	entry *entries= calloc(noOfEntries,sizeof(entry));

	char *lineBuf = NULL; 
	size_t lineSize;
	int lineLength;

	FILE *sFile = fopen(subject,"r");
	if(!sFile)
		bomb("Entries not downloaded, check network connection");
	getline(&lineBuf, &lineSize, sFile);
	for(i=1;i<=lineCount;i++){
	/*extracts entries line by line, identifying type by line count*/
		getline(&lineBuf, &lineSize, sFile);
		lineLength = strlen(lineBuf);
		if(i>1)
			sw = (i-2)%5;
		else
			sw = 9;
		switch(sw){
			case 0:
				strcpy(entries[entryNo].arxivNo,lineBuf);
				entries[entryNo].arxivNo[lineLength-1]='\0';
				break;
			case 1:
				strcpy(entries[entryNo].title,lineBuf);
				entries[entryNo].title[lineLength-1]='\0';
				break;
			case 2:
				strcpy(entries[entryNo].authors,lineBuf);
				entries[entryNo].authors[lineLength-1]='\0';
				break;
			case 3:
				strcpy(entries[entryNo].abstract,lineBuf);
				entries[entryNo].abstract[lineLength-1]='\0';
				break;
			case 4:
				entryNo++;
				break;
		}
	}
	fclose(sFile);
	free(lineBuf);
	return(entries);
}

void *navigator(void *argStruct ){

	nav_arg_struct *args = argStruct; 

	WINDOW *titleWin = args->tWin;
	WINDOW *authWin = args->aWin;
	WINDOW *absWin = args->abWin;

	int *input = args->inp;
	bool *chsub = args->changeSubject;

	FILE *savedEntries;
	int lineCount = lineCounter(CUR_SUBJECT);
	int noOfEntries = lineCount/5;
	int nolTitle, nolAbs, nolAuths; 
	/*numbers of lines for title,author and abstract*/

	int entryNo;

	int titleWinH, titleWinW, authWinH, authWinW, absWinW, absWinH;

	/*read in dimensions of windows*/
	getmaxyx(titleWin,titleWinH,titleWinW);
	getmaxyx(authWin,authWinH,authWinW);
	getmaxyx(absWin,absWinH,absWinW);
	/*reduce to size of window content*/
	titleWinH = titleWinH-3;
	titleWinW = titleWinW-4;
	authWinH = authWinH -3;
	authWinW = authWinW -4;
	absWinH= absWinH-3;
	absWinW = absWinW-4;

	//Entry data to display
	entries_header *head = fetchHeader(CUR_SUBJECT);
	entryNo = head->curEntryNo;

	bool fin=0; /*True when end of subject area reached*/

	entry *entries = fetchEntries(CUR_SUBJECT,noOfEntries);

	int absScrl=0, authScrl = 0;

	mvprintw(4,40,"(%s)",head->dateF);
	refresh();

	//Entries have been fetched - can now rename file
	
	bool ran=1; /*True when arxiv number needs to be refreshed*/

	char **titleLines = NULL;
	char **absLines = NULL;
	char **authLines = NULL;

	savedEntries = fopen(SAVED_ENTRIES,"a");

	fprintf(savedEntries,
		"{\"subject\": \"%s\" ,\"datef\": \"%s\"}\n",
		head->subject,head->dateF);

	while(!fin)
	{
		//refreshing displayed windows
		updateEntryCount(entryNo,noOfEntries);

		titleLines = toLines(entries[entryNo-1].title,titleWinW,
				&nolTitle); 
		updateWin(titleWin,titleLines,nolTitle, 0, "Window too small");
		free2DArray(titleLines);

		authLines =  commaSplit(entries[entryNo-1].authors,
				&nolAuths, authWinW);
		updateWin(authWin,authLines,nolAuths,authScrl,
						"Use PgUp/PgDn to scroll");
		free2DArray(authLines);

		absLines = toLines(entries[entryNo-1].abstract,
							absWinW,&nolAbs); 
		updateWin(absWin,absLines,nolAbs, absScrl, 
				"Use arrowkeys to scroll");
		free2DArray(absLines);

		if(ran){
			attron(A_BOLD);
			move(4,28);
			clearText(10);
			move(4,28);
			typewriter(entries[entryNo-1].arxivNo,40);
			attroff(A_BOLD);
			ran=0;
		}

		wrefresh(titleWin);
		wrefresh(authWin);
		wrefresh(absWin);

		flushinp();

		*input = getch();

		switch(*input){ 
		/*controls abstract scroll value, changes entry number*/
			case KEY_BACKSPACE:
				entryNo= entryNo+1;
				if(entryNo>noOfEntries){
					*chsub = 1;
					fin = 1;
				}
				absScrl = 0;
				authScrl = 0;
				ran = 1;
				break;
			case '\n':
				fprintf(savedEntries,
					"\n%s\n%s\n%s\n%s\n",
					entries[entryNo-1].arxivNo,
					entries[entryNo-1].title,
					entries[entryNo-1].authors,
					entries[entryNo-1].abstract);
				entryNo=entryNo+1;
				if(entryNo>noOfEntries){
					*chsub = 1;
					fin = 1;
				}
				absScrl = 0;
				authScrl = 0;
				ran = 1;
				break;
			case 'u':
				entryNo=entryNo-1;
				ran = 1;
				if(entryNo==0){
					entryNo =1;		
					ran=0;
				}
				authScrl = 0;
				break;
			case KEY_DOWN:
				absScrl = newScrollValue(absWin,nolAbs,
								absScrl+1);
				ran = 0;
				break;
			case KEY_UP:
				absScrl = newScrollValue(absWin,nolAbs,
								absScrl-1);
				ran = 0;
				break;
			case KEY_NPAGE:
				authScrl = newScrollValue(authWin,nolAuths,
								authScrl+1);
				ran = 0;
				break;
			case KEY_PPAGE:
				authScrl = newScrollValue(authWin,nolAuths,
							       	  authScrl-1);
				ran = 0;
				break;
			case 's':
				fin = 1;
			case 'q':
				fin = 1;
			case '0':
				ran=1;
				break;
			default:
				ran=0;
				break;
		}
	}
	fclose(savedEntries);
	free(entries);
	head->curEntryNo = entryNo;
	setHeader(head);
	free(head);
	return NULL;
}

/****************************************************************************/
/**                                                                        **/
/**                               EOF                                      **/
/**                                                                        **/
/****************************************************************************/

