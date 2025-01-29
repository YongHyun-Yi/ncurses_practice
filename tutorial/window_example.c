
// #include <ncurses.h>


// WINDOW *create_newwin(int height, int width, int starty, int startx);
// void destroy_win(WINDOW *local_win);

// int main(int argc, char *argv[])
// {	WINDOW *my_win;
// 	int startx, starty, width, height;
// 	int ch;

// 	initscr();			/* Start curses mode 		*/
// 	cbreak();			/* Line buffering disabled, Pass on
// 					 * everty thing to me 		*/
// 	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/

// 	height = 3;
// 	width = 10;
// 	starty = (LINES - height) / 2;	/* Calculating for a center placement */
// 	startx = (COLS - width) / 2;	/* of the window		*/
// 	printw("Press F1 to exit");
// 	refresh();
// 	my_win = create_newwin(height, width, starty, startx);

// 	while((ch = getch()) != KEY_F(1))
// 	{	switch(ch)
// 		{	case KEY_LEFT:
// 				destroy_win(my_win);
// 				my_win = create_newwin(height, width, starty,--startx);
// 				break;
// 			case KEY_RIGHT:
// 				destroy_win(my_win);
// 				my_win = create_newwin(height, width, starty,++startx);
// 				break;
// 			case KEY_UP:
// 				destroy_win(my_win);
// 				my_win = create_newwin(height, width, --starty,startx);
// 				break;
// 			case KEY_DOWN:
// 				destroy_win(my_win);
// 				my_win = create_newwin(height, width, ++starty,startx);
// 				break;	
// 		}
// 	}
		
// 	endwin();			/* End curses mode		  */
// 	return 0;
// }

// WINDOW *create_newwin(int height, int width, int starty, int startx)
// {	WINDOW *local_win;

// 	local_win = newwin(height, width, starty, startx);
// 	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
// 					 * for the vertical and horizontal
// 					 * lines			*/
// 	wrefresh(local_win);		/* Show that box 		*/

// 	return local_win;
// }

// void destroy_win(WINDOW *local_win)
// {	
// 	/* box(local_win, ' ', ' '); : This won't produce the desired
// 	 * result of erasing the window. It will leave it's four corners 
// 	 * and so an ugly remnant of window. 
// 	 */
// 	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
// 	/* The parameters taken are 
// 	 * 1. win: the window on which to operate
// 	 * 2. ls: character to be used for the left side of the window 
// 	 * 3. rs: character to be used for the right side of the window 
// 	 * 4. ts: character to be used for the top side of the window 
// 	 * 5. bs: character to be used for the bottom side of the window 
// 	 * 6. tl: character to be used for the top left corner of the window 
// 	 * 7. tr: character to be used for the top right corner of the window 
// 	 * 8. bl: character to be used for the bottom left corner of the window 
// 	 * 9. br: character to be used for the bottom right corner of the window
// 	 */
// 	wrefresh(local_win);
// 	delwin(local_win);
// }

#include <ncurses.h>

typedef struct _win_border_struct {
	chtype 	ls, rs, ts, bs, 
	 	tl, tr, bl, br;
}WIN_BORDER;

typedef struct _WIN_struct {

	int startx, starty;
	int height, width;
	WIN_BORDER border;
}WIN;

void init_win_params(WIN *p_win);
void print_win_params(WIN *p_win);
void create_box(WIN *win, bool flag);

int main(int argc, char *argv[])
{	WIN win;
	int ch;

	initscr();			/* Start curses mode 		*/
	start_color();			/* Start the color functionality */
	cbreak();			/* Line buffering disabled, Pass on
					 * everty thing to me 		*/
	keypad(stdscr, TRUE);		/* I need that nifty F1 	*/
	noecho();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);

	/* Initialize the window parameters */
	init_win_params(&win);
	print_win_params(&win);

	attron(COLOR_PAIR(1));
	printw("Press F1 to exit");
	refresh();
	attroff(COLOR_PAIR(1));
	
	create_box(&win, TRUE);
	while((ch = getch()) != KEY_F(1))
	{	switch(ch)
		{	case KEY_LEFT:
				create_box(&win, FALSE);
				--win.startx;
				create_box(&win, TRUE);
				break;
			case KEY_RIGHT:
				create_box(&win, FALSE);
				++win.startx;
				create_box(&win, TRUE);
				break;
			case KEY_UP:
				create_box(&win, FALSE);
				--win.starty;
				create_box(&win, TRUE);
				break;
			case KEY_DOWN:
				create_box(&win, FALSE);
				++win.starty;
				create_box(&win, TRUE);
				break;	
		}
	}
	endwin();			/* End curses mode		  */
	return 0;
}
void init_win_params(WIN *p_win)
{
	p_win->height = 3;
	p_win->width = 10;
	p_win->starty = (LINES - p_win->height)/2;	
	p_win->startx = (COLS - p_win->width)/2;

	p_win->border.ls = '|';
	p_win->border.rs = '|';
	p_win->border.ts = '-';
	p_win->border.bs = '-';
	p_win->border.tl = '+';
	p_win->border.tr = '+';
	p_win->border.bl = '+';
	p_win->border.br = '+';

}
void print_win_params(WIN *p_win)
{
#ifdef _DEBUG
	mvprintw(25, 0, "%d %d %d %d", p_win->startx, p_win->starty, 
				p_win->width, p_win->height);
	refresh();
#endif
}
void create_box(WIN *p_win, bool flag)
{	int i, j;
	int x, y, w, h;

	x = p_win->startx;
	y = p_win->starty;
	w = p_win->width;
	h = p_win->height;

	if(flag == TRUE)
	{	mvaddch(y, x, p_win->border.tl);
		mvaddch(y, x + w, p_win->border.tr);
		mvaddch(y + h, x, p_win->border.bl);
		mvaddch(y + h, x + w, p_win->border.br);
		mvhline(y, x + 1, p_win->border.ts, w - 1);
		mvhline(y + h, x + 1, p_win->border.bs, w - 1);
		mvvline(y + 1, x, p_win->border.ls, h - 1);
		mvvline(y + 1, x + w, p_win->border.rs, h - 1);

	}
	else
		for(j = y; j <= y + h; ++j)
			for(i = x; i <= x + w; ++i)
				mvaddch(j, i, ' ');
				
	refresh();

}