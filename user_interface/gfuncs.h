/*gfuncs.h*/

extern int max(int a,int b);
extern int min(int a,int b);
extern void bomb(char *msg);
extern void colours();
extern void typewriter(char *typed_text, int timestep);
extern int line_counter(char *filename);
extern WINDOW *bwinc(char *title, char *content, int height, int width, int ypos,int xpos);
extern int new_scroll_value(WINDOW *win,char *content,int scv);
extern void update_win(WINDOW *win,char *new_content,int scm);
extern void clear_text(int length);
extern void pull_datum(char **datum,size_t *datum_size,char *buffer);
extern void title_screen(void);
extern void path_to_sub(int item,size_t *ftring_size,char **file_string);
extern void make_menu(WINDOW *mwin,char *path_to_file, int item, int scl);
extern void ecount_update(int ent_count,int no_of_entries);
extern void refresh_menu(WINDOW *menu_win, char* path_to_file,int m_input,int *mm, int *sv);
extern void xtag(WINDOW *win,int xd_terms[]);
extern void smenu(WINDOW *win,char *fpath,int sitem, int ymax,int xmax,int yy,int xx);
//extern int fetch_entry(FILE *f,int line_size,
//				char *arxiv_no,char *title,char *authors,char *abstract);
