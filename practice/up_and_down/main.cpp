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
#define DEFAULT_BEST 100


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

int ans, upto, downto, score, BEST = 100, game_state;
int selected_num = -1;

WINDOW *w_canvas;
WINDOW *w_title;
WINDOW *w_background;
WINDOW *w_number;
WINDOW *w_score;
WINDOW *w_updown;
WINDOW *w_info;

PANEL *p_canvas;
PANEL *p_title;
PANEL *p_background;
PANEL *p_number;
PANEL *p_score;
PANEL *p_updown;
PANEL *p_info;

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

void draw_info2()
{
	mvwprintw(w_info, 2, 2, "THIS IS TEST");
	mvwprintw(w_info, 3, 2, "HELLO WORLD");
}

void draw_updown2()
{
	mvwprintw(w_updown, 0, 3, "UP");
	mvwprintw(w_updown, 1, 2, "DOWN");
}

void draw_score2()
{
	int offset_score = 0;

	mvwprintw(w_score, 1, 2, "best");
	snprintf(buf, BUFMAX, "%d", BEST);
	if (BEST < 10)
		offset_score = 1;
	else if (BEST == DEFAULT_BEST)
	{
		snprintf(buf, BUFMAX, "-");
		offset_score = 1;
	}
	mvwprintw(w_score, 1, 12 + offset_score, buf);
	mvwprintw(w_score, 1, 15, "try");

	mvwprintw(w_score, 2, 2, "score");
	snprintf(buf, BUFMAX, "%d", score);
	if (score < 10)
		offset_score = 1;
	mvwprintw(w_score, 2, 12 + offset_score, buf);
	mvwprintw(w_score, 2, 15, "try");
}

// 상황별로 다른 서식을 받는다
void draw_nums2(int from, int to, t_format fmt)
{
	int offset_num_x = 1, offset_num_y = 1;

	apply_format(fmt);
	
	for (int num = from; num <= to; num++)
	{
		int x, y;

		x = num % 10 + offset_num_x;
		y = num / 10 + offset_num_y;
		snprintf(buf, BUFMAX, "%.2d", num);
		mvwprintw(w_number, y, x * (CHAR_WIDTH + CHAR_MARGIN), buf);
	}

	remove_format(fmt);
}

// update 에서 변경된 값을 토대로
// 각 panel 들의 렌더링 요소를 갱신
void draw_game2()
{
	draw_nums2(downto + 1, upto - 1, normal_num);
	draw_score2();
	draw_updown2();
	draw_info2();
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
			if (score < BEST)
			{
				BEST = score;
				snprintf(buf, BUFMAX, "Congratulations!!! you've got high score!");
				mvprintw(3, 0, buf);
			}
			snprintf(buf, BUFMAX, "High Score: %d try", BEST);
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

void create_panels()
{
	// canvas
	{
		w_canvas = newwin(MAXHEIGHT, MAXWIDTH, 0, 0);
		p_canvas = new_panel(w_canvas);
		box(w_canvas, 0, 0);
	}

	// title
	{
		int offset_x = 2, offset_y = 3;
	
		w_title = newwin(12, 64, 0, 0);
		p_title = new_panel(w_title);
	
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
	}

	// game screen contents
	{
		int offset_x_ingame = 6, offset_y_ingame = 1;
	
		w_background = newwin(15, 54, 0, 0);
		w_number = newwin(12, 33, 0, 0);
		w_score = newwin(4, 20, 0, 0);
		w_updown = newwin(2, 8, 0, 0);
		w_info = newwin(6, 20, 0, 0);
	
		p_background = new_panel(w_background);
		p_number = new_panel(w_number);
		p_score = new_panel(w_score);
		p_updown = new_panel(w_updown);
		p_info = new_panel(w_info);
	
		// background
		mvwprintw(w_background, 0, 0, "Up and Down");
		mvwprintw(w_background, 0, 41, "by. YHY_GAMES");
		mvwprintw(w_background, 1, 0, "──────────────────────────────────────────────────────");
		mvwprintw(w_background, 14, 0, "──────────────────────────────────────────────────────");
		move_panel(p_background, offset_y_ingame, offset_x_ingame);
		hide_panel(p_background);
	
		// number board
		box(w_number, 0, 0);
		move_panel(p_number, 2 + offset_y_ingame, offset_x_ingame);
		hide_panel(p_number);
	
		// score board
		box(w_score, 0, 0);
		move_panel(p_score, 2 + offset_y_ingame, 34 + offset_x_ingame);
		hide_panel(p_score);
	
		// up down
		mvwprintw(w_updown, 0, 3, "UP");
		mvwprintw(w_updown, 1, 2, "DOWN");
		move_panel(p_updown, 6 + offset_y_ingame, 40 + offset_x_ingame);
		hide_panel(p_updown);
	
		// info board
		box(w_info, 0, 0);
		move_panel(p_info, 8 + offset_y_ingame, 34 + offset_x_ingame);
		hide_panel(p_info);
	}

	update_panels();
	doupdate();
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

void test()
{
	//
}

int main()
{
	// 초기화를 진행합니다.
	init();

	create_panels();

	getch();

	hide_panel(p_title);
	
	game_settup();

	// 게임화면 표시
	// show_panel(p_background);
	// show_panel(p_number);
	// show_panel(p_score);
	// show_panel(p_updown);
	// show_panel(p_info);

	// draw_games2();

	// update_panels();
	// doupdate();

	draw_game();
	getch();

	run();
}