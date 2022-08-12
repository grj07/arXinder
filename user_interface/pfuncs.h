/*pfuncs.h*/

extern void *call_datprc();
extern int colours();
extern void *title_screen(int col);
extern WINDOW *bt_win(char *title, int height, int width, int ypos,int xpos);
extern char *subject(int n);
extern void ecount_update(int ent_count,int no_of_entries);
extern void update_win(WINDOW *win,char *new_content, int scm);
extern char *fetch_entry(char* sub,int entry_no, int line_count,int etype);
extern char *make_menu(WINDOW *mwin,char *path_to_file,int item, int scl);
extern char *refresh_menu(WINDOW *menu_win,char* path_to_file,int m_input, int *mm, int *sv);
extern void smenu(WINDOW *s_win,char * sub);
extern void category_menu(WINDOW *menu_win);
extern void navigator(WINDOW *twin,WINDOW *authwin,WINDOW *abswin,char *sub, int input, int *entry_no, int *abs_scrl);
extern void chosen_subjects(WINDOW *cho_win, int scl);
extern void clear_sublist();
