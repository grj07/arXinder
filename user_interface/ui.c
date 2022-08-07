#include <ncurses.h> /*ncurses library, includes stdio.h*/
#include <math.h>    /*Floor and ceilings*/
#include <string.h>  /*string manipulations str...*/
#include <stdlib.h>  /*dynamic memory allocation malloc*/
#include "gfuncs.h"

#define VERSION "0.0.3"

int main(int argc, char *argv[])
{
	//Windows to display
	WINDOW *my_win, *title_win, *auth_win, *abs_win, *menu_win;
	char chsub[] = "s - change subjects"; 
	size_t ptf_size, qx_size;
	char *pathtf;

	//Declarations for reading entry data from file
	FILE *f_sublist, *f_entries; 
	char *ent_buf, *subj_buf; /*buffers store lines of file reading*/
	size_t ent_buf_size, subj_buf_size; /*Sizes of buffer necessary for 
					      storing strings to buffer with getline*/
	ssize_t line_size, subj_size;
	//Entry data to display
	char *sub, *entry_arxivno, *entry_title, *entry_abstract, *entry_authors;

	//Reading subject list
	f_sublist = fopen("subjects.conf", "rw");
	subj_size = getline(&subj_buf,&subj_buf_size,f_sublist);
	sub = (char *)malloc(strlen(subj_buf)-1);
	strncpy(sub,subj_buf,strlen(subj_buf)-1);
	fclose(f_sublist);

	//Computing the number of entries
	int line_count = line_counter(sub);
	int no_of_entries = (line_count-1)/5;	
	int ent_count=1;           /*start on first entry*/

	size_t seekch[line_count]; /*Array to store line sizes */
	line_count = 0;
	int seek_ch=0;		   /*Seek backwards to previous entries
					by skipping back seekch characters*/

	//Switches which track what data type the buffer is reading
	int swarx = 1, swtit = 0, swauth = 0, swabs = 0;
	int mm = 0, km=0, sv = 0;

	//Taking input from the user
	int input, m_input;
	int abs_scrl = 0;
	//Start ncurses
	initscr();			

	keypad(stdscr, TRUE);		
	colours();

	//Title screen on opening
	title_screen();
	move(2,10);
	attrset(COLOR_PAIR(2));
	typewriter("v ",100);
	typewriter(VERSION,100);

	mvprintw(4,COLS-strlen(chsub),chsub);

	title_win = bwinc("Title","", 8,COLS/2-1,5,2);
	napms(100);
	auth_win = bwinc("Authors", "",8,COLS/2-1,5,COLS/2+2);
	napms(100);
	abs_win = bwinc("Abstract","", LINES-13,COLS-2,13,2);
	wattrset(abs_win,COLOR_PAIR(2));
	mvwaddstr(abs_win,1,COLS-26,"Use arrowkeys to scroll");
	napms(100);
	move(0,15);
	flushinp();

	//Subject bar defines current subject shown
	attrset(COLOR_PAIR(4)|A_BOLD);
	mvprintw(2,20,sub);
	attrset(COLOR_PAIR(4));
	ecount_update(ent_count,no_of_entries);
	

	/* Get the first line of the file. */
	f_entries = fopen(sub,"r");
	line_size = getline(&ent_buf, &ent_buf_size, f_entries);
	seekch[0] = line_size;
	line_size = getline(&ent_buf, &ent_buf_size, f_entries);
	seekch[line_count+1] = line_size;
	/*while loop navigates through entries*/
	while(line_size >= 0){
		if( swtit ==1){
			size_t etitle_size;
			pull_datum(&entry_title,&etitle_size,ent_buf);
			swtit=0;
			swauth=1;
		}
		else if( swauth ==1){
			size_t eauth_size;
			pull_datum(&entry_authors,&eauth_size,ent_buf);
			swauth=0;
			swabs=1;
		}
		else if( swabs ==1){
			size_t eabs_size;
			pull_datum(&entry_abstract,&eabs_size,ent_buf);
			swabs=0;
			//At this point the entire entry has been copied, 
			//and can be printed as the contents
			//of the window
			update_win(title_win,entry_title,0);
			update_win(auth_win,entry_authors,0);
			update_win(abs_win,entry_abstract,0);
			move(4,28);
			clear_text(10);
			move(4,28);
			typewriter(entry_arxivno,40);
			input = getch();
			if(input == KEY_DOWN |input ==  KEY_UP |input ==  's')
			{
				do
				{
					switch(input)
					{
						case KEY_DOWN:
							abs_scrl = new_scroll_value(abs_win,entry_abstract,abs_scrl+1);
							update_win(abs_win,entry_abstract,abs_scrl);
							input = getch();
							break;
						case KEY_UP:
							abs_scrl = new_scroll_value(abs_win,entry_abstract,abs_scrl-1);
							update_win(abs_win,entry_abstract,abs_scrl);
							input = getch();
							break;
						case 's':
							curs_set(0);
							noecho();
							menu_win = newwin(0,0,5,1);
							mm = 0;
							sv = 0;
							make_menu(menu_win,"cat_list", mm,sv);
							wrefresh(menu_win);
							m_input = getch();
							while (m_input != 'q') 
							{
								refresh_menu(menu_win,"cat_list",m_input,&mm,&sv);
								if(m_input == '\n')
								{
									for(km=0;km<8;km++)
									{
										if(km == mm)
										{
											path_to_sub(km,&ptf_size,&pathtf);
										}
									}
									smenu(menu_win,pathtf,0,0,0,1,COLS/2);
									make_menu(menu_win,"cat_list", mm,sv);
									wrefresh(menu_win);
								}
								wrefresh(menu_win);
								m_input = getch();
							}
							delwin(menu_win);
							touchwin(title_win);
							touchwin(auth_win);
							touchwin(abs_win);
							wrefresh(title_win);
							wrefresh(auth_win);
							wrefresh(abs_win);
							input = getch();
							break;
					}
				}while(input == KEY_DOWN |input ==  KEY_UP |input ==  's');
			}
			if(input ==KEY_RIGHT)
			{
				abs_scrl = 0;
				line_count++;
				swarx = line_count-1;
				swarx = swarx % 5;
				ent_count++;
				line_size = getline(&ent_buf, &ent_buf_size, f_entries);
				seekch[line_count+1] = line_size;
				ecount_update(ent_count,no_of_entries);
				continue;
			}	
			else if(input ==KEY_LEFT)
			{
				abs_scrl = 0;
				if(ent_count==1)
				{
					break;
				}
				else{
/*this code counts how */		seek_ch=seekch[line_count+1]+
/*many characters the cursor*/			seekch[line_count]+seekch[line_count-1]+
/*skips back to the previous entry*/		seekch[line_count-2]+seekch[line_count-3]+
						seekch[line_count-4];
					line_count=line_count-5;
					seek_ch=seek_ch +seekch[line_count]+seekch[line_count-1]+
						seekch[line_count-2]+seekch[line_count-3]+
						seekch[line_count-4];
					line_count=line_count-5;
					fseek(f_entries,-seek_ch,SEEK_CUR);
					line_size = getline(&ent_buf, &ent_buf_size, f_entries);
					line_count++;
					swarx = line_count-1;
					swarx = swarx % 5;
					ent_count = ent_count-1;
					line_size = getline(&ent_buf, &ent_buf_size, f_entries);
					seekch[line_count+1] = line_size;
					ecount_update(ent_count,no_of_entries);
					continue;
				}
			}
			else
			{
				break;
			}
		}
		else if( swarx==0){
			size_t e_arxivno_size;
			pull_datum(&entry_arxivno,&e_arxivno_size,ent_buf);
			swtit = 1;
	}
	line_count++;
	swarx = line_count-1;
	swarx = swarx % 5;
	line_size = getline(&ent_buf, &ent_buf_size, f_entries);
	seekch[line_count+1] = line_size;
	}
	fclose(f_entries);
	endwin();			/* End curses mode		  */
	return 0;
}
