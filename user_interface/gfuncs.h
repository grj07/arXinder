/*gfuncs.h*/

extern void bomb(char *msg);
extern void colours();
extern void typewriter(char *typed_text, int timestep);
extern int line_counter(char *filename);
extern WINDOW *bwinc(char *title, char *content, int height, int width, int ypos,int xpos);
extern void update_win(WINDOW *win,char *new_content);
extern void clear_text(int length);
extern void pull_datum(char **datum,size_t *datum_size,char *buffer);
extern void title_screen(void);
extern void subject_menu(void);
extern void ecount_update(int ent_count,int no_of_entries);
//extern int fetch_entry(FILE *f,int line_size,
//				char *arxiv_no,char *title,char *authors,char *abstract);
