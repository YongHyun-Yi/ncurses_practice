#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include <locale.h>
#include <ncurses.h>
#include <panel.h>

#include <sstream>

#define MAXWIDTH 68
#define MAXHEIGHT 17

#define CHAR_WIDTH 2
#define CHAR_MARGIN 1
#define BUFMAX 512
#define DEFAULT_UPTO 100
#define DEFAULT_DOWNTO -1


// 서식 구조체 정의
typedef struct s_format {
	attr_t attrs;
	short color;
} t_format;

// 출력용 char 배열
char buf[BUFMAX];

// 전역 서식 변수 정의
t_format normal_num = { 0, 1 };
t_format excluded_num = { 0, 2 };
t_format wrong_num = { 0, 3 };
t_format correct_num = { 0, 4 };

int ans, upto, downto, score, highscore = 100, game_state;
int selected_num = -1;

void init();
void game_settup();

// 디버그 출력
// 마우스 위치
// 정답 숫자
// 클릭 된 숫자
// 정답보다 큰지 작은지

// 서식 적용 함수
void apply_format(t_format fmt)
{
	if (fmt.attrs) attron(fmt.attrs);
	if (fmt.color) attron(COLOR_PAIR(fmt.color));
}

// 서식 해제 함수
void remove_format(t_format fmt)
{
	if (fmt.attrs) attroff(fmt.attrs);
	if (fmt.color) attroff(COLOR_PAIR(fmt.color));
}

// 상황별로 다른 서식을 받는다
void draw_nums(int from, int to, t_format fmt)
{
	apply_format(fmt);
	
	for (int num = from; num <= to; num++)
	{
		int x, y;

		x = num % 10;
		y = num / 10;
		snprintf(buf, BUFMAX, "%.2d", num);
		mvprintw(y, x * (CHAR_WIDTH + CHAR_MARGIN), buf);
	}

	remove_format(fmt);
}

// 상태값에 따라 화면을 렌더링
// 0: 초기
// 1: 오답, 작음
// 2: 오답, 큼
// 3: 정답
void draw_game()
{
	snprintf(buf, BUFMAX, "Try: %d", score);
	mvprintw(11, 0, buf);

	switch (game_state)
	{
		case 0:
			draw_nums(downto, upto - 1, normal_num);
			break;
		
		case 1:
			draw_nums(downto + 1, selected_num - 1, excluded_num);
			draw_nums(selected_num, selected_num, wrong_num);
			downto = selected_num;
			break;
		
		case 2:
			draw_nums(selected_num + 1, upto - 1, excluded_num);
			draw_nums(selected_num, selected_num, wrong_num);
			upto = selected_num;
			break;

		case 3:
			clear();
			snprintf(buf, BUFMAX, "Game Over");
			mvprintw(0, 0, buf);
			snprintf(buf, BUFMAX, "answer is: %.2d", ans);
			mvprintw(1, 0, buf);
			if (score < highscore)
			{
				highscore = score;
				snprintf(buf, BUFMAX, "Congratulations!!! you've got high score!");
				mvprintw(3, 0, buf);
			}
			snprintf(buf, BUFMAX, "High Score: %d try", highscore);
			mvprintw(5, 0, buf);
			snprintf(buf, BUFMAX, "Score: %d try", score);
			mvprintw(6, 0, buf);
			snprintf(buf, BUFMAX, "Press 'R' key to Retry game");
			mvprintw(8, 0, buf);
			break;
	}

	selected_num = -1;
}

void update_game()
{
	if (selected_num == -1)
		return;
	
	++score;

	if (selected_num < ans)
		game_state = 1;
	else if (selected_num > ans)
		game_state = 2;
	else
		game_state = 3;
}


void click_handler(MEVENT &mevent)
{
	int x, y, num;

	// 공백 클릭은 무시한다
	if (mevent.x % 3 == 2)
		return ;

	x = mevent.x / (CHAR_WIDTH + CHAR_MARGIN);
	y = mevent.y * 10;
	num = x + y;

	// 제외된 범위 클릭은 무시한다
	if (num <= downto || num >= upto)
		return ;
	
	selected_num = num;
}

void handle_input()
{
	int c = getch();

	switch(c)
	{
		// 마우스 이벤트 처리
		// 이벤트 종류에 따라 분기
		case KEY_MOUSE:
			MEVENT mevent;

			if (getmouse(&mevent) == OK)
			{
				if (mevent.bstate & BUTTON1_CLICKED)
					click_handler(mevent);
			}
			break;
		
		// 재시작
		case 'r':
			clear();
			game_settup();
			break;
		
		// 종료
		case 'q':
			endwin();
			exit(0);
	}
}

void run()
{
	while(1)
	{
		handle_input();
		update_game();
		draw_game();
	}
}

void game_settup()
{
	// 0 ~ 99 사이의 무작위 숫자를 생성합니다.
	srand(time(0));
	ans = rand() % 100 + 1;

	// 상한선, 하한선, 점수 변수 초기화
	upto = DEFAULT_UPTO;
	downto = DEFAULT_DOWNTO;
	score = 0;
	game_state = 0;
}

void init()
{
	// ncurses 의 초기화 함수들을 설정합니다.
	setlocale(LC_CTYPE, "");
	initscr();
	curs_set(0);
	noecho();
	keypad(stdscr, TRUE);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

	start_color();
	init_color(8, 500, 500, 500); // A_DIM 대신 회색을 추가한다
	init_pair(1, COLOR_BLACK, COLOR_WHITE); // Normal Number
	init_pair(2, 8, COLOR_BLACK); // Exclude Number
	init_pair(3, COLOR_BLACK, COLOR_RED); // Wrong Number
	init_pair(4, COLOR_BLACK, COLOR_GREEN); // Correct Number

	refresh();
}

/*

 Up and Down                              by. YHY_GAMES
 ──────────────────────────────────────────────────────
 ┌───────────────────────────────┐ ┌──────────────────┐
 │ 00 01 02 03 04 05 06 07 08 09 │ │ best       4 try │
 │ 10 11 12 13 14 15 16 17 18 19 │ │ score      1 try │
 │ 20 21 22 23 24 25 26 27 28 29 │ └──────────────────┘
 │ 30 31 32 33 34 35 36 37 38 39 │          up
 │ 40 41 42 43 44 45 46 47 48 49 │       < down >
 │ 50 51 52 53 54 55 56 57 58 59 │ ┌──────────────────┐
 │ 60 61 62 63 64 65 66 67 68 69 │ │                  │
 │ 70 71 72 73 74 75 76 77 78 79 │ │    52  greater   │
 │ 80 81 82 83 84 85 86 87 88 89 │ │   than answer    │
 │ 90 91 92 93 94 95 96 97 98 99 │ │                  │
 └───────────────────────────────┘ └──────────────────┘
 ──────────────────────────────────────────────────────

( 56 x 17 )
*/
void draw_placeholder()
{
	int offset_x_ingame = 6, offset_y_ingame = 1;

	WINDOW *w_background = newwin(15, 54, 0, 0);
	WINDOW *w_number = newwin(12, 33, 0, 0);
	WINDOW *w_score = newwin(4, 20, 0, 0);
	WINDOW *w_updown = newwin(2, 8, 0, 0);
	WINDOW *w_info = newwin(6, 20, 0, 0);

	PANEL *p_background = new_panel(w_background);
	PANEL *p_number = new_panel(w_number);
	PANEL *p_score = new_panel(w_score);
	PANEL *p_updown = new_panel(w_updown);
	PANEL *p_info = new_panel(w_info);

	// background
	mvwprintw(w_background, 0, 0, "Up and Down");
	mvwprintw(w_background, 0, 41, "by. YHY_GAMES");
	mvwprintw(w_background, 1, 0, "──────────────────────────────────────────────────────");
	mvwprintw(w_background, 14, 0, "──────────────────────────────────────────────────────");
	move_panel(p_background, offset_y_ingame, offset_x_ingame);
	
	// number board
	int offset_num_x = 1, offset_num_y = 1;
	mvwprintw(w_number, 0, 0, "┌───────────────────────────────┐");
	for (int i = 1; i < 11; i++)
	{
		mvwprintw(w_number, i, 0, "│");
		mvwprintw(w_number, i, 32, "│");
	}
	mvwprintw(w_number, 11, 0, "└───────────────────────────────┘");
	for (int num = 0; num <= 99; num++)
	{
		int x, y;

		x = num % 10 + offset_num_x;
		y = num / 10 + offset_num_y;
		snprintf(buf, BUFMAX, "%.2d", num);
		mvwprintw(w_number, y, x * (CHAR_WIDTH + CHAR_MARGIN), buf);
	}
	move_panel(p_number, 2 + offset_y_ingame, offset_x_ingame);

	// score board
	mvwprintw(w_score, 0, 0, "┌──────────────────┐");
	for (int i = 1; i < 3; i++)
	{
		mvwprintw(w_score, i, 0, "│");
		mvwprintw(w_score, i, 19, "│");
	}
	mvwprintw(w_score, 3, 0, "└──────────────────┘");
	move_panel(p_score, 2 + offset_y_ingame, 34 + offset_x_ingame);

	// up down
	mvwprintw(w_updown, 0, 3, "UP");
	mvwprintw(w_updown, 1, 2, "DOWN");
	move_panel(p_updown, 6 + offset_y_ingame, 40 + offset_x_ingame);

	// info board
	mvwprintw(w_info, 0, 0, "┌──────────────────┐");
	for (int i = 1; i < 5; i++)
	{
		mvwprintw(w_info, i, 0, "│");
		mvwprintw(w_info, i, 19, "│");
	}
	mvwprintw(w_info, 5, 0, "└──────────────────┘");
	move_panel(p_info, 8 + offset_y_ingame, 34 + offset_x_ingame);

	update_panels();
	doupdate();
	getch();
}

void draw_title()
{
	int offset_x = 2, offset_y = 3;

	WINDOW *w_title = newwin(12, 64, 0, 0);
	PANEL *p_title = new_panel(w_title);

	const char *title[] = {
		"   __  __         ___              __   ____                    ",
		"  / / / /___     /   |  ____  ____/ /  / __ \\____ _      ______ ",
		" / / / / __ \\   / /| | / __ \\/ __  /  / / / / __ \\ | /| / / __ \\",
		"/ /_/ / /_/ /  / ___ |/ / / / /_/ /  / /_/ / /_/ / |/ |/ / / / /",
		"\\____/ .___/  /_/  |_/_/ /_/\\__,_/  /_____/\\____/|__/|__/_/ /_/ ",
		"    /_/                                                         "

	};

	for (int i = 0; i < 6; i++)
	{
		mvwprintw(w_title, i, 0, "%s", title[i]);
	}
	mvwprintw(w_title, 7, 49, "by. YHY_GAMES");
	attron(A_BLINK);
	mvwprintw(w_title, 11, 22, "Press AnyKey to Start");
	attroff(A_BLINK);
	
	move_panel(p_title, offset_y, offset_x);
	update_panels();
	doupdate();

	getch();

	del_panel(p_title);
	delwin(w_title);
	update_panels();
	doupdate();
}

void test()
{
	setlocale(LC_CTYPE, "");
	initscr();
	curs_set(0);
	noecho();
	
	// refresh();

	WINDOW *win;
	PANEL *panel;

	win = newwin(5, 7, 3, 3);
	panel = new_panel(win);
	box(win, 0, 0);
	mvwprintw(win, 1, 2, "This");
	mvwprintw(win, 2, 3, "is");
	mvwprintw(win, 3, 2, "win3");
	// wrefresh(win);
	update_panels();
	doupdate();

	getch();

	move_panel(panel, 3, 6);
	// mvwin(win, 3, 6);
	// wrefresh(win);
	update_panels();
	doupdate();

	getch();
	
	endwin();
	exit(0);
}

int main()
{
	// 초기화를 진행합니다.
	init();

	// 캔버스 경계 확인용 창을 생성합니다.
	WINDOW *w_canvas = newwin(MAXHEIGHT, MAXWIDTH, 0, 0);
	PANEL *p_canvas = new_panel(w_canvas);
	box(w_canvas, 0, 0);
	update_panels();
	doupdate();

	// 타이틀 화면 표시
	draw_title();
	
	draw_placeholder();
	getch();
	
	// 게임화면 표시
	clear();
	game_settup();
	draw_game();

	// 계속해서 반복되는 핵심 부분입니다.
	run();
}