/*gfuncs.h*/

extern void bomb(char *msg);			        /*exits with error message*/
extern int min(int a,int b);			        /*Maximum of two integers*/
extern int max(int a,int b);			        /*Minimum of two integers*/
extern void typewriter(char *typed_text, int timestep); /*Types out from cursor position with gap timestep in milliseconds*/
extern void add_colour(int ncp,int qu); 		/*sets colour ncp if qu true (!=0)*/
extern void rem_colour(int ncp,int qu);			/*removes colour ncp if qu true (!=0)*/
extern int line_counter(char *filename);		/*counts the number of lines in file*/
extern void clear_text(int length);			/*erases length chars from from cursor position*/
extern int new_scroll_value(WINDOW *win,int tot_lines,int scv); /*filter to stop scroll at start/end of content*/
extern WINDOW *bt_win(char *title, int height, int width, int ypos,int xpos); /*Bordered window to be filled with content*/
extern void update_win(WINDOW *win,char **new_content, int no_of_lines,int scm, char * scrl_inst);
//Updates the contents (length no_of_lines multidimensional line array of a bordered window (win), 
//scm is scroll value. scrlinst is scrolling instruction string
extern char *join_strings(char *string_1, char *string_2);	/*Concatenates string1 and string2*/
extern char **to_lines(char *content, int line_width,int* no_of_lines); /*Returns pointer to array of length 
									  no_of_lines with entries strings of width line_width.*/
extern void free_lines(char **lines_buf);			/*free multidim array*/
