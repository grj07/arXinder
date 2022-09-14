/*pfuncs.c - particular functions for arXinder*/
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <jansson.h>
#include "pfuncs.h"
#include "gfuncs.h"

#define GRAY 8  
#define GRAYR 500
#define GRAYG 500
#define GRAYB 500

#define LRED 9
#define LREDR 1000
#define LREDG 800
#define LREDB 800

#define LYELLOW 10 
#define LYELLOWR 855
#define LYELLOWG 855
#define LYELLOWB 700


struct _entries_header{
	char subject[20];
	int raw_no_of_entries;
	char date[9];
	char datef[18];
	int cur_entry_no;
	int filtered_no_of_entries;
};

struct _entry
{
	char arxiv_no[12];
	char title[300];
	char authors[3000];
	char abstract[2000];
};

//Navigator argument structure needed to thread navigator with python script
struct _nav_arg_struct{
	WINDOW *twin;
       	WINDOW *awin;
       	WINDOW	*abwin;
	int * inp;
	bool *change_subject;
};

int colours() /*Colours checks if the terminal can do colours, and returns 1 if so*/
{
	int qu=0;
	if(!has_colors())
		bomb("Terminal cannot do colours!");
	else if(start_color()!=OK)
		bomb("Unable to start colours!");
	else if(!can_change_color())
		bomb("Can't change color");
	else
	{
	init_color(LRED,LREDR,LREDG,LREDB);
	init_color(GRAY,GRAYR,GRAYG,GRAYB);
	init_color(LYELLOW,LYELLOWR,LYELLOWG,LYELLOWB);
	init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(2,GRAY,COLOR_BLACK);
	init_pair(3,LRED,COLOR_BLACK);
	init_pair(4,LYELLOW,COLOR_BLACK);
	init_pair(5,COLOR_RED,LYELLOW);
	qu = 1;
	}	
	return(qu);
}


void update_subs(int n, char *sub_array[],int nsubs) /*updates the subject display*/
{		
	int jj=0;
	int ll=0;
	int dm, ctot;
	ctot = strlen(sub_array[n])+5;

	jj = n+1;
	for(jj= n+1;jj <nsubs+n;jj++)
	{
		ll = jj % nsubs;
		ctot = ctot + max(strlen(sub_array[ll])+5,0);
	}
	move(2,20);
	clear_text(ctot);
	attron(A_BOLD);
	mvprintw(2,20,sub_array[n]);
	attroff(A_BOLD);
	dm = strlen(sub_array[n])+5;
	for(jj=n+1;jj<nsubs+n;jj++)
	{
		ll = jj % nsubs;
		mvprintw(2,20+dm,sub_array[ll]);
		dm = dm + max(strlen(sub_array[ll])+5,0);
	}
}

void ecount_update(int ent_count,int no_of_entries)/*updates the entry number displayed*/
{
	move(3,19);
	clear_text(9);
	if(ent_count<10)
		mvprintw(3,21,"%d/%d",ent_count,no_of_entries);
	else if(ent_count < 100)
		mvprintw(3,20,"%d/%d",ent_count,no_of_entries);
	else
		mvprintw(3,19,"%d/%d",ent_count,no_of_entries);
}


entries_header *fetch_header(char *sub){ /*loads header as json data through jansson*/
	entries_header *head = malloc(sizeof(entries_header));	
	json_t* jtemp = NULL;
	
	char * line_buf=NULL;
	size_t l_size;
	json_error_t error;
	FILE *efile;
	
	efile = fopen(sub,"r");

	getline(&line_buf, &l_size, efile);
	json_t *root = json_loads(line_buf,0,&error);
	jtemp = json_object_get(root, "subject");
	strcpy(head->subject,(char*) json_string_value(jtemp));
	jtemp = json_object_get(root, "raw_no_of_entries");
	head->raw_no_of_entries = (int) json_integer_value(jtemp);
	jtemp = json_object_get(root, "date");
	strcpy(head->date,(char*) json_string_value(jtemp));
	jtemp = json_object_get(root, "datef");
	strcpy(head->datef,(char*) json_string_value(jtemp));
	jtemp = json_object_get(root, "cur_entry_no");
	head->cur_entry_no = (int) json_integer_value(jtemp);
	jtemp = json_object_get(root, "filtered_no_of_entries");
	head->filtered_no_of_entries = (int) json_integer_value(jtemp);

	fclose(efile);
	free(jtemp);
	free(line_buf);
	return(head);
}

//to be run upon the exit of navigator. Running from navigator most sensible as a fin condition
//need an intermediate buffer to copy entries from (the file itself?)
void set_header(entries_header *head){ /*saves header to file*/
	char filepath[] = "state/p_feeds/";
	char * file_str;
	char * fstr;
	json_t* jtemp = NULL;
	char* new_head = NULL;
	
	char * line_buf=NULL;
	size_t l_size;
	ssize_t l_length;
	FILE *efile;
	FILE *wfile;
	

	json_t* root = json_object(); 
	jtemp = json_string(head->subject);
	json_object_set_new(root, "subject", jtemp);
	jtemp = json_integer(head->raw_no_of_entries);
	json_object_set_new(root, "raw_no_of_entries", jtemp);
	jtemp = json_string(head->date);
	json_object_set_new(root, "date", jtemp);
	jtemp = json_string(head->datef);
	json_object_set_new(root, "datef", jtemp);
	jtemp = json_integer(head->cur_entry_no);
	json_object_set_new(root, "cur_entry_no", jtemp);
	jtemp = json_integer(head->filtered_no_of_entries);
	json_object_set_new(root, "filtered_no_of_entries", jtemp);

	fstr = join_strings(filepath,head->subject);
	file_str = join_strings(fstr,head->date);
	new_head = json_dumps(root,0);
	wfile = fopen(file_str,"w");
	fputs(new_head,wfile);
	fputs("\n",wfile);

//efile is buffer file, renamed version of current subject file
	efile = fopen("state/cursub","r");
	l_length = getline(&line_buf, &l_size, efile);
	l_length = getline(&line_buf, &l_size, efile);
	while(l_length>= 0)
	{
		fputs(line_buf,wfile);
		l_length = getline(&line_buf, &l_size, efile);
	}
	fclose(efile);
	fclose(wfile);
	json_decref(root);
	free(fstr);
	free(file_str);
	free(new_head);
	free(line_buf);
}

entry *fetch_entries(char *subject,int no_of_entries)/*populates array of entry structs with entry data*/
{
	FILE *s_file; /*subject file containing entries*/
	int line_count = 5*no_of_entries+1;

	entry *entries= calloc(no_of_entries,sizeof(entry));

	char *line_buffer = NULL; /*line buffer is filled with line from file using getline*/
	size_t lb_size;
	int line_length;

	int kk, typ;
	int ent_count = 0;
	s_file = fopen(subject,"r");
	if(!s_file)
		bomb("error: cannot find entries, please check network connection");
	//loop through entries, putting arxiv number, authors, etc in appropriate struct 
	getline(&line_buffer, &lb_size, s_file);
	for(kk=1;kk<=line_count;kk++) 
	{
		getline(&line_buffer, &lb_size, s_file);
		line_length = strlen(line_buffer);
		if(kk>1)
			typ = (kk-2)%5;
		else
			typ = 9;
		switch(typ)
		{
			case 0:
				strcpy(entries[ent_count].arxiv_no,line_buffer);
				entries[ent_count].arxiv_no[line_length-1]='\0';
				break;
			case 1:
				strcpy(entries[ent_count].title,line_buffer);
				entries[ent_count].title[line_length-1]='\0';
				break;
			case 2:
				strcpy(entries[ent_count].authors,line_buffer);
				entries[ent_count].authors[line_length-1]='\0';
				break;
			case 3:
				strcpy(entries[ent_count].abstract,line_buffer);
				entries[ent_count].abstract[line_length-1]='\0';
				break;
			case 4:
				ent_count++;
				break;
		}
	}
	fclose(s_file);
	free(line_buffer);
	return(entries);
}

char **auth_strtol(char *auth_str, int* no_of_auths, int width)/*returns array of authors from comma sep string*/
{

	char *auth_buf=malloc(sizeof(char)*(strlen(auth_str)+1));
	strcpy(auth_buf,auth_str);
	auth_buf[strlen(auth_str)]='\0';
	char ch = auth_buf[0];
	int kk = 1, ak=1;
	while(ch)
	{
		ch = auth_buf[kk];
		if(ch == ',')
			ak++;
		kk++;
	}
	char **auth_list=malloc(sizeof(char *)*ak) ;
	const char delim[2] = ",";
	char *auth_token = NULL;
	char *auth = calloc(ak*width,sizeof(char));
	int strl;	
	auth_token = strtok(auth_buf,delim);
	for(kk=0;kk<ak;kk++)
	{
		strl = min(strlen(auth_token),width);
		strncpy(auth+kk*width,auth_token,strl);
		*(auth+kk*(width)+strl)='\0';
		*(auth_list+kk) = auth+kk*(width);
		auth_token = strtok(NULL,delim);
	}
	*no_of_auths = ak;
	free(auth_buf);
	return(auth_list);
}


//Navigator browses arxiv entries
void * navigator(void *arg_struct ){
	nav_arg_struct *args = arg_struct; 
	WINDOW *twin = args->twin;
	WINDOW *authwin = args->awin;
	WINDOW *abswin = args->abwin;
	int * input = args->inp;
	bool *chsub = args->change_subject;
	char temp[] = "state/cursub";


	FILE *saved_entries;
	int line_count = line_counter(temp);
	int no_of_entries = line_count/5;
	int nol_tit, nol_abs, nol_auths;
	int entry_no;

	//Reading the parameters of the windows for accurate display
	int authwin_h, authwin_w, twin_h,twin_w, abswin_w,abswin_h;
	getmaxyx(twin,twin_h,twin_w);
	getmaxyx(authwin,authwin_h,authwin_w);
	getmaxyx(abswin,abswin_h,abswin_w);
	twin_h = twin_h-3;
	twin_w = twin_w-4;
	authwin_h = authwin_h -3;
	authwin_w = authwin_w -4;
	abswin_h= abswin_h-3;
	abswin_w = abswin_w-4;

	//Entry data to display
	entries_header *head = fetch_header(temp);
	entry_no = head->cur_entry_no;

	

	//If it has been read, move on straight away
	bool fin=0; /*True when end of subject area reached*/

	entry *entries = fetch_entries(temp,no_of_entries);
	int abs_scrl=0, auth_scrl = 0;

	mvprintw(4,40,"(%s)",head->datef);
	refresh();
	//Entries have been fetched - can now rename file
	
	bool ran=1; /*True when arxiv number needs to be refreshed*/

	char ** title_lines = NULL;
	char ** abs_lines = NULL;
	char ** auth_lines = NULL;
	saved_entries = fopen("state/saved_entries.d","a");
	fprintf(saved_entries,"{\"subject\": \"%s\" ,\"datef\": \"%s\"}\n",head->subject,head->datef);
	while(!fin)
	{
		//refreshing displayed windows
		ecount_update(entry_no,no_of_entries);

		title_lines = to_lines(entries[entry_no-1].title,twin_w,&nol_tit); 
		update_win(twin,title_lines,nol_tit, 0, "Window too small");
		free_lines(title_lines);

		auth_lines =  auth_strtol(entries[entry_no-1].authors,&nol_auths, authwin_w);
		update_win(authwin,auth_lines,nol_auths,auth_scrl,"Use PgUp/PgDn to scroll");
		free_lines(auth_lines);

		abs_lines = to_lines(entries[entry_no-1].abstract,abswin_w,&nol_abs); 
		update_win(abswin,abs_lines,nol_abs, abs_scrl, "Use arrowkeys to scroll");
		free_lines(abs_lines);

		if(ran)
		{
			attron(A_BOLD);
			move(4,28);
			clear_text(10);
			move(4,28);
			typewriter(entries[entry_no-1].arxiv_no,40);
			attroff(A_BOLD);
			ran=0;
		}
		wrefresh(twin);
		wrefresh(authwin);
		wrefresh(abswin);

		flushinp();
		*input = getch();
		switch(*input) /*Updates abstract  sroll value, changes entry number*/
		{
			case KEY_BACKSPACE:
				entry_no= entry_no+1;
				if(entry_no>no_of_entries){
					*chsub = 1;
					fin = 1;
				}
				abs_scrl = 0;
				auth_scrl = 0;
				ran = 1;
				break;
			case '\n':
				fprintf(saved_entries,"\n%s\n%s\n%s\n%s\n",entries[entry_no-1].arxiv_no,
						entries[entry_no-1].title,entries[entry_no-1].authors,
									entries[entry_no-1].abstract);
				entry_no=entry_no+1;
				if(entry_no>no_of_entries){
					*chsub = 1;
					fin = 1;
				}
				abs_scrl = 0;
				auth_scrl = 0;
				ran = 1;
				break;
			case 'u':
				entry_no=entry_no-1;
				ran = 1;
				if(entry_no==0){
					entry_no =1;		
					ran=0;
				}
				auth_scrl = 0;
				break;
			case KEY_DOWN:
				abs_scrl = new_scroll_value(abswin,nol_abs,abs_scrl+1);
				ran = 0;
				break;
			case KEY_UP:
				abs_scrl = new_scroll_value(abswin,nol_abs,abs_scrl-1);
				ran = 0;
				break;
			case KEY_NPAGE:
				auth_scrl = new_scroll_value(authwin,nol_auths,auth_scrl+1);
				ran = 0;
				break;
			case KEY_PPAGE:
				auth_scrl = new_scroll_value(authwin,nol_auths,auth_scrl-1);
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
	fclose(saved_entries);
	free(entries);
	head->cur_entry_no = entry_no;
	set_header(head);
	free(head);
	return NULL;
}

void chosen_subjects(WINDOW *cho_win,int scl)/*displays subjects chosen from subjects.conf*/
{
	FILE *sub_list;
	int kk=0;
	int wh, ww;
	char *buf=NULL;
	size_t buf_size; 
	ssize_t lsize;
	getmaxyx(cho_win,wh,ww);
	char *menu_item = calloc(ww-1,sizeof(char));
	sub_list = fopen("config/subjects.conf","r");
	lsize = getline(&buf,&buf_size,sub_list);
	while(lsize >=0)	
	{
		strcpy(menu_item,buf);
		*(menu_item+strlen(buf)-1)='\0';

	      	wmove(cho_win,kk-scl,0);
		wclrtoeol(cho_win);
		if(kk>=scl)
			mvwaddstr(cho_win,kk-scl,1,menu_item);
		wattroff(cho_win,A_REVERSE);
		lsize = getline(&buf,&buf_size,sub_list);
		if(kk>wh+scl)
			break;
		kk++;
	}
	fclose(sub_list);
	free(buf);
	free(menu_item);
}

void clear_sublist()/*clears subjects (empties file)*/
{
	FILE *sublist;
	sublist = fopen("config/subjects.conf","w");
	fclose(sublist);
}

//make_menu prints an instant of the window to mwin, and returns name of
//menu item for use in file path
char *make_menu(WINDOW *mwin,char *path_to_file,int item, int scl)
{
	FILE *menu_list; /*menu list stored in individual files*/
	int kk=0;
	int wh, ww;
	char *buf=NULL;
	char *sel_item = NULL;
	size_t buf_size; 
	ssize_t lsize;

	getmaxyx(mwin,wh,ww);
	size_t menu_item_size=(ww-1)*sizeof(char);
	char *menu_item = calloc(ww-1,sizeof(char));

	menu_list = fopen(path_to_file,"r");

	lsize = getline(&buf,&buf_size,menu_list);
	while(lsize >=0)	
	{
		strncpy(menu_item,buf,ww-1);
		*(menu_item+min(strlen(buf)-1,ww-1))='\0';/*no pesky newline character*/

	      	wmove(mwin,kk-scl,0);/*clear old menu item in this spot*/
		wclrtoeol(mwin);
		if(kk==item) /*highlight item selected, and copy for output*/
		{
			sel_item = (char *)malloc(menu_item_size);
			strcpy(sel_item,menu_item);
			wattron(mwin,A_REVERSE);
		}
		if(kk>=scl)/*only print the menu items that can fit in window*/
			mvwaddstr(mwin,kk-scl,1,menu_item);
		wattroff(mwin,A_REVERSE);
		lsize = getline(&buf,&buf_size,menu_list);
		if(kk>wh+scl)
			break;
		kk++;
	}
	fclose(menu_list);

	free(buf);
	free(menu_item);
	return(sel_item); /*memory freed in main*/
}

//refresh_menu responds to minput and refreshes the new menu
char *refresh_menu(WINDOW *menu_win,char* path_to_file,int m_input, int *mm, int *sv)
{
	int wh, lc; /*need to know: window dims and number of lines for menu*/
	char *categ;

	lc = line_counter(path_to_file);
	wh = getmaxy(menu_win);

	switch(m_input) /* adjusts menu according to item selected and scroll value*/
	{
		case KEY_DOWN:
			*mm = *mm+1;
			if(*mm>=lc)
			{
				*mm=0;
				*sv=0;
			}
			if(lc<wh)
				*sv=0;
			else if(*mm>=wh-*sv)
				*sv = min(*sv+1,lc-wh);
			break;
		case KEY_UP:
			*mm = *mm-1;
			if(*mm<0)
			{
				*mm=lc-1;
				*sv=lc-wh;
			}
			if(lc<wh)
				*sv=0;
			else if(*sv>0 && *mm<*sv)
				*sv = *sv-1;
			break;
		case '\n':
			*mm = *mm;
			*sv = *sv;
			break;
	}

	categ =make_menu(menu_win,path_to_file, *mm,*sv);	
	return(categ); /*memory freed in main*/
}

bool s_menu() /*main subject menu, returns true if program needs to restart (possible changes to subjects)*/
{
	FILE *sub_file;
	WINDOW *menu_win, *cat_win, *sub_win, *cho_win; 
	char smsub[] = "c - clear subjects";  
	int cm = 0,csv = 0, sm = 0, ssv = 0;
	int input;
	bool changes_made=0;
	char *cat, *sbj;
	char *path = malloc(60*sizeof(char));
	char *tpath = malloc(60*sizeof(char));

	menu_win=bt_win("Subject Settings",LINES-5,COLS-4,5,2);
	wbkgd(menu_win,COLOR_PAIR(2));
	mvwaddstr(menu_win,1,COLS-25,smsub);
	wrefresh(menu_win);
	wbkgd(menu_win,COLOR_PAIR(4));
	cat_win = derwin(menu_win,9,45,2,1); 
	sub_win = derwin(menu_win,LINES-8,COLS-60,2,48);
	cho_win = derwin(menu_win,LINES-20,45,14,1);
	chosen_subjects(cho_win,0);
		
	wrefresh(cat_win);
	wrefresh(cho_win);
	cat = make_menu(cat_win,"cat_list",cm,csv);
	wrefresh(cat_win);
	input = getch();
	while(input!='q') /*choose category*/
	{	
		free(cat);
		cat = refresh_menu(cat_win,"cat_list",input,&cm,&csv);

		if(input == 'c')
		{
			clear_sublist();
			wclear(cho_win);
			chosen_subjects(cho_win,0);
			wrefresh(cho_win);
			changes_made = 1;
		}
		if(input=='\n')/*category selected, choose subject*/
		{
			
			free(path);
			path = join_strings("subjects/",cat);

			//fetch array with subject settings
			free(tpath);
			tpath = join_strings("subjects/",cat);

			sbj = make_menu(sub_win,path,sm,ssv);
			wrefresh(sub_win);
			input = getch();
			while(input!='q')
			{
				if(input == 'c')
				{
					clear_sublist();
					wclear(cho_win);
					chosen_subjects(cho_win,0);
					wrefresh(cho_win);
				}
				if(input == 'c')
					clear_sublist();
				if(input == '\n')/*subject selected, add to list*/
				{
					sub_file=fopen("config/subjects.conf","a");
					fprintf(sub_file,"%s\n",strtok(sbj ," ,"));
					fclose(sub_file);
					chosen_subjects(cho_win,0);
					wrefresh(cho_win);
					changes_made = 1;
				}
				free(sbj);
				sbj = refresh_menu(sub_win,path,input,&sm,&ssv);
				wrefresh(sub_win);
				input = getch();
			}
			free(sbj);
		}
		sm = 0;
		ssv = 0;
		wclear(sub_win);
		wrefresh(sub_win);
		wrefresh(cat_win);
		input = getch();
	}
	wclear(menu_win);
	wrefresh(menu_win);
	delwin(menu_win);
	free(cat);
	free(path);
	free(tpath);
	move(0,5);
	clear_text(40);
	refresh();
	return(changes_made);
}
