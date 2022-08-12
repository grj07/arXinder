/*gfuncs.h*/

extern void bomb(char *msg);
extern int min(int a,int b);
extern int max(int a,int b);
extern void typewriter(char *typed_text, int timestep);
extern void add_colour(int ncp,int qu);
extern void rem_colour(int ncp,int qu);
extern int line_counter(char *filename);
extern void pull_datum(char **datum,size_t *datum_size,char *buffer);
extern void clear_text(int length);
extern int new_scroll_value(WINDOW *win,char *content,int scv);
extern char *join_strings(char *string_1, char *string_2);
