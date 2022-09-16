/*pfuncs.h*/

struct _entries_header; /*structs for entry header from file - Read from json using jansson*/
struct _entry;          /*entries from file*/
struct _nav_arg_struct; /*needed to pass arguments to threaded function (navigator)*/

typedef struct _entries_header entries_header;
typedef struct _entry entry;
typedef struct _nav_arg_struct nav_arg_struct;

/*****refresh_screen.c******/
extern int colours(); /*Colours checks if the terminal can do colours, and returns 1 if so*/
extern void update_subs(int n, char *sub_array[],int nsubs);/*updates the subject display*/
extern void ecount_update(int ent_count,int no_of_entries);/*updates the entry number displayed*/
/*manage_entries.c*/
extern entries_header *fetch_header(char *sub);/*loads header as json data through jansson*/
extern void set_header(entries_header *head);/*saves header to file*/
extern entry *fetch_entries(char *subject,int no_of_entries);/*populates array of entry structs with entry data*/
/*utils.c*/
extern char **auth_strtol(char *auth_str, int* no_of_auths, int width);/*returns array of authors from comma sep string*/
/*nav_and_sort.c*/
extern void * navigator(void *arg_struct );/*browse arxiv entries*/


/*******Menu.c******/
extern void clear_sublist();/*clears subjects (empties file)*/
extern void chosen_subjects(WINDOW *cho_win, int scl);/*displays subjects chosen from subjects.conf*/
extern char *make_menu(WINDOW *mwin,char *path_to_file,int item, int scl);
//prints an instant of the window to mwin, and returns name of for use in file path
extern char *refresh_menu(WINDOW *menu_win,char* path_to_file,int m_input, int *mm, int *sv);
//refresh_menu responds to minput and refreshes the new menu
extern bool s_menu();/*main subject menu, returns true if program needs to restart (possible changes to subjects)*/
