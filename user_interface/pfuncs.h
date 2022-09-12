/*pfuncs.h*/

struct _entries_header;
struct _entry;
struct _nav_arg_struct;

typedef struct _entries_header entries_header;
typedef struct _entry entry;
typedef struct _nav_arg_struct nav_arg_struct;

extern int colours();
extern WINDOW *bt_win(char *title, int height, int width, int ypos,int xpos);
extern void update_subs(int n, char *sub_array[],int nsubs);
extern void ecount_update(int ent_count,int no_of_entries);
extern void update_win(WINDOW *win,char *new_content, int scm);
extern void update_winb(WINDOW *win,char **new_content, int no_of_lines,int scm, char * scrl_inst);
extern char *fetch_entry(char* sub,int entry_no, int line_count,int etype);
extern char *make_menu(WINDOW *mwin,char *path_to_file,int item, int scl);
extern char *refresh_menu(WINDOW *menu_win,char* path_to_file,int m_input, int *mm, int *sv);
extern entries_header *fetch_header(char *sub);
extern void set_header(entries_header *head);
extern entry *fetch_entries(char *subject,int no_of_entries);
extern void category_menu(WINDOW *menu_win);
extern char **auth_strtol(char *auth_str, int* no_of_auths, int width);
extern void * navigator(void *arg_struct );
extern void chosen_subjects(WINDOW *cho_win, int scl);
extern void clear_sublist();
extern void s_menu();