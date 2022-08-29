/*pfuncs.c - particular functions for arXinder*/
#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
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


//Bordered window with content
WINDOW *bt_win(char *title, int height, int width, int ypos,int xpos)
{	
	WINDOW *f_win;

	f_win = newwin(height,width,ypos,xpos);
	wbkgd(f_win,COLOR_PAIR(3));
	box(f_win, 0 , 0);		
	mvwprintw(f_win,1,width/2-strlen(title)/2,title);
	return f_win;
}

void update_subs(int n, char *sub_array[],int nsubs) /*returns the subject to extract entries from*/
{
	int jj=0;
	int ll=0;
	int dm, ctot;
	ctot = strlen(sub_array[n])+5;
	for(jj=n+1;jj<nsubs+n;jj++)
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

void ecount_update(int ent_count,int no_of_entries)
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

//Updates the window with a content window, scm is scroll value 
void update_win(WINDOW *win,char *new_content, int scm)
{
	WINDOW *c_win;
	int height, width;
	int npl, nchars;
	int c, cmax, cmin;
	char ch;

	getmaxyx(win,height,width); 	/*measures size of window*/
	npl = width-4;			/*number of characters per line*/
	nchars=(height-3)*(width-4);    /*number of characters in the window*/
	cmax = min(strlen(new_content)-1,nchars); /*last char printed to window*/

	if(cmax == nchars) /*case where too much content for window*/
	{
		cmin = max(npl*scm,0); /*cmin is first char*/
	}
	else
		cmin =0;

	cmax = min(cmax +cmin,strlen(new_content)-1);/*last char calculated*/

	c_win = derwin(win,height-3,width-4,2,2);
	wbkgd(c_win,COLOR_PAIR(4));
	wclear(c_win);
	//wattron(c_win,A_BOLD);
	wmove(c_win,0,0);
	for(c=cmin;c<=cmax;c++)
	{
		ch = (char)*(new_content+c);
		waddch(c_win,ch);
	}
	//wattroff(c_win,A_BOLD);
	wrefresh(c_win);
	wrefresh(win);
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

typedef struct entry
{
	char arxiv_no[12];
	char title[300];
	char authors[300][50];
	char abstract[2000];
}entry;

entry *fetch_entries(char *subject,int no_of_entries)
{
	FILE *s_file; /*subject file containing entries*/
	int line_count = 5*no_of_entries+1;

	entry *entries= calloc(no_of_entries,sizeof(entry));

	char *line_buffer = NULL; /*line buffer is filled with line from file using getline*/
	char *auth_token = NULL;
	char authbuf[3000];
	const char delim[2] = ",";
	size_t lb_size;
	int line_length;

	int kk, jj, typ;
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
				strcpy(authbuf,line_buffer);
				authbuf[line_length-1]='\0';
				auth_token = strtok(authbuf,delim);
				refresh();
				jj = 0;
				while(auth_token!=NULL)
				{
					strcpy(entries[ent_count].authors[jj],auth_token);
					entries[ent_count].authors[jj][strlen(auth_token)]='\0';
					auth_token = strtok(NULL,delim);
					jj++;
				}
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
	return(entries);
}
//Navigator browses arxiv entries
bool navigator(WINDOW *twin,WINDOW *authwin,WINDOW *abswin,char *sub, int *input,int *entry_no)
{
	FILE *saved_entries;
	int line_count = line_counter(sub);
	int no_of_entries = line_count/5;

	//Reading the parameters of the windows for accurate display
	int authwin_h, authwin_w;
	getmaxyx(authwin,authwin_h,authwin_w);
	authwin_h = authwin_h -3;
	authwin_w = authwin_w -4;
	char auth_str[authwin_h*authwin_w];
	int kk, jj, atot, auth_l;

	//Entry data to display
	entry *entries = fetch_entries(sub,no_of_entries);
	int abs_scrl=0;

	bool ran=1; /*True when arxiv number needs to be refreshed*/
	bool fin=0; /*True when end of subject area reached*/
	bool chsub=0;/*True when the entries are finished*/

	saved_entries = fopen("saved_entries.d","a");
	while(!fin)
	{
		//refreshing displayed windows
		ecount_update(*entry_no,no_of_entries);
		update_win(twin,entries[*entry_no-1].title, abs_scrl);
		atot = 0;
		for(kk=0;kk<authwin_h;kk++)
		{
			if(!entries[*entry_no-1].authors[kk][0])
				break;
			auth_l = strlen(entries[*entry_no-1].authors[kk]);
			for(jj=0;jj<auth_l;jj++)
			{
				auth_str[atot+jj]=entries[*entry_no-1].authors[kk][jj];
			}
			auth_str[atot+auth_l]='\n';
			atot = atot + auth_l+1;
		}
		auth_str[atot] = '\0';
		update_win(authwin,auth_str,abs_scrl);
		update_win(abswin,entries[*entry_no-1].abstract,abs_scrl);
		if(ran)
		{
			attron(A_BOLD);
			move(4,28);
			clear_text(10);
			move(4,28);
			typewriter(entries[*entry_no-1].arxiv_no,40);
			attroff(A_BOLD);
			ran=0;
		}
		wrefresh(twin);
		wrefresh(authwin);
		wrefresh(abswin);

		*input = getch();
		switch(*input) /*Updates abstract  sroll value, changes entry number*/
		{
			case KEY_BACKSPACE:
				*entry_no= *entry_no+1;
				if(*entry_no>no_of_entries)
				{
					chsub = 1;
					fin = 1;
				}
				abs_scrl = 0;
				ran = 1;
				break;
			case '\n':
				fprintf(saved_entries,"%s\n",entries[*entry_no-1].arxiv_no);
				*entry_no=*entry_no+1;
				if(*entry_no>no_of_entries)
				{
					chsub = 1;
					fin = 1;
				}
				abs_scrl = 0;
				ran = 1;
				break;
			case KEY_DOWN:
				abs_scrl = new_scroll_value(abswin,entries[*entry_no-1].abstract,abs_scrl+1);
				ran = 0;
				break;
			case KEY_UP:
				abs_scrl = new_scroll_value(abswin,entries[*entry_no-1].abstract,abs_scrl-1);
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
	return(chsub);
}

void chosen_subjects(WINDOW *cho_win,int scl)
{
	FILE *sub_list;
	int kk=0;
	int wh, ww;
	char *buf=NULL;
	size_t buf_size; 
	ssize_t lsize;
	getmaxyx(cho_win,wh,ww);
	char *menu_item = calloc(ww-1,sizeof(char));
	sub_list = fopen("subjects.conf","r");
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

void clear_sublist()
{
	FILE *sublist;
	sublist = fopen("subjects.conf","w");
	fclose(sublist);
}

void s_menu()
{
	FILE *sub_file;
	WINDOW *menu_win, *cat_win, *sub_win, *cho_win; 
	char smsub[] = "c - clear subjects";  
	int cm = 0,csv = 0, sm = 0, ssv = 0;
	int input;
	char *cat, *sbj;
	char *path = malloc(60*sizeof(char));
	char *tpath = malloc(60*sizeof(char));

	move(0,5);
	printw("Please restart program to enact changes");
	refresh();
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
					sub_file=fopen("subjects.conf","a");
					fprintf(sub_file,"%s\n",strtok(sbj ," ,"));
					fclose(sub_file);
					chosen_subjects(cho_win,0);
					wrefresh(cho_win);
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
}
