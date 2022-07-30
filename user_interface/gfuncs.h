/*gfuncs.h*/

extern void bomb(char *msg);
extern void typewriter(char *typed_text, int timestep);
extern void title_screen(void);
extern WINDOW *bwinc(char *title, char *content, int height, int width, int ypos,int xpos);
extern void colours();
extern void update_win(WINDOW *win,char *new_content);
