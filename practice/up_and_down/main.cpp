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
t_format highlight_num = { 0, 5 };

int ans, upto, downto, score, best_score = 100;
int selected_num = -1;
bool game_over, is_greater, is_correct, is_init;

WINDOW *w_canvas;

WINDOW *w_background;
WINDOW *w_number;
WINDOW *w_score;
WINDOW *w_updown;
WINDOW *w_info;

PANEL *p_canvas;

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
void apply_format(WINDOW *win, t_format fmt)
{
	if (fmt.attrs) wattron(win, fmt.attrs);
	if (fmt.color) wattron(win, COLOR_PAIR(fmt.color));
}

// 서식 해제 함수
void remove_format(WINDOW *win, t_format fmt)
{
	if (fmt.attrs) wattroff(win, fmt.attrs);
	if (fmt.color) wattroff(win, COLOR_PAIR(fmt.color));
}

void draw_info()
{
	werase(w_info);
	box(w_info, 0, 0);

	// info
	if (is_init)
		return;

	// 정답
	if (is_correct)
	{
		apply_format(w_info, correct_num);
		snprintf(buf, BUFMAX, "%d", selected_num);
		mvwprintw(w_info, 2, 5 + (selected_num < 10), buf);
		remove_format(w_info, correct_num);
		
		snprintf(buf, BUFMAX, "is");
		mvwprintw(w_info, 2, 8, buf);

		snprintf(buf, BUFMAX, "the answer");
		mvwprintw(w_info, 3, 4, buf);
	}
	// 오답
	else
	{
		apply_format(w_info, wrong_num);
		snprintf(buf, BUFMAX, "%d", selected_num);
		mvwprintw(w_info, 2, 5 + (selected_num < 10), buf);
		remove_format(w_info, wrong_num);

		if (is_greater)
		{
			snprintf(buf, BUFMAX, "greater");
			mvwprintw(w_info, 2, 9, buf);
		}
		else
		{
			snprintf(buf, BUFMAX, "lesser");
			mvwprintw(w_info, 2, 8, buf);
		}
		snprintf(buf, BUFMAX, "than answer");
		mvwprintw(w_info, 3, 4, buf);
	}
}

void draw_updown()
{
	werase(w_updown);

	// updown
	if (is_init || is_correct)
	{
		// 정답
		apply_format(w_updown, excluded_num);
		mvwprintw(w_updown, 0, 1, "  UP  ");
		mvwprintw(w_updown, 1, 0, "  DOWN  ");
		remove_format(w_updown, excluded_num);
	}
	else
	{
		if (is_greater)
		{
			// down
			apply_format(w_updown, excluded_num);
			mvwprintw(w_updown, 0, 1, "  UP  ");
			remove_format(w_updown, excluded_num);

			apply_format(w_updown, normal_num);
			mvwprintw(w_updown, 1, 0, "  DOWN  ");
			remove_format(w_updown, normal_num);

			apply_format(w_updown, highlight_num);
			mvwprintw(w_updown, 1, 0, "<");
			mvwprintw(w_updown, 1, 7, ">");
			remove_format(w_updown, highlight_num);
		}
		else
		{
			// up
			apply_format(w_updown, normal_num);
			mvwprintw(w_updown, 0, 1, "  UP  ");
			remove_format(w_updown, normal_num);

			apply_format(w_updown, excluded_num);
			mvwprintw(w_updown, 1, 0, "  DOWN  ");
			remove_format(w_updown, excluded_num);

			apply_format(w_updown, highlight_num);
			mvwprintw(w_updown, 0, 1, "<");
			mvwprintw(w_updown, 0, 6, ">");
			remove_format(w_updown, highlight_num);
		}
	}
}

void draw_score()
{
	werase(w_score);
	box(w_score, 0, 0);

	// score
	mvwprintw(w_score, 1, 2, "best");
	mvwprintw(w_score, 1, 15, "try");

	mvwprintw(w_score, 2, 2, "score");
	mvwprintw(w_score, 2, 15, "try");

	apply_format(w_score, highlight_num);

	if (best_score == DEFAULT_BEST)
		mvwprintw(w_score, 1, 13, "-");
	else
	{
		snprintf(buf, BUFMAX, "%d", best_score);
		mvwprintw(w_score, 1, 12 + (best_score < 10), buf);
	}
	
	snprintf(buf, BUFMAX, "%d", score);
	mvwprintw(w_score, 2, 12 + (score < 10), buf);
	
	remove_format(w_score, highlight_num);
}

// ------------------------------------------------------------

// 상황별로 다른 서식을 받는다
void draw_nums(int from, int to, t_format fmt)
{
	int offset_num_x = 1, offset_num_y = 1;

	apply_format(w_number, fmt);
	
	for (int num = from; num <= to; num++)
	{
		int x, y;

		x = num % 10 + offset_num_x;
		y = num / 10 + offset_num_y;
		snprintf(buf, BUFMAX, "%.2d", num);
		mvwprintw(w_number, y, x * (CHAR_WIDTH + CHAR_MARGIN), buf);
	}

	remove_format(w_number, fmt);
}

void draw_game()
{
	// number
	if (is_init)
	{
		draw_nums(downto + 1, upto - 1, normal_num);
	}
	else if (is_correct)
	{
		// 정답
		werase(w_number);
		box(w_number, 0, 0);
		draw_nums(DEFAULT_DOWNTO + 1, DEFAULT_UPTO - 1, excluded_num);
		draw_nums(selected_num, selected_num, correct_num);
	}
	else
	{
		// 오답
		draw_nums(selected_num, selected_num, wrong_num);
		if (is_greater)
		{
			// 더 큰 숫자
			draw_nums(selected_num + 1, upto - 1, excluded_num);
			upto = selected_num;
		}
		else
		{
			// 더 작은 숫자
			draw_nums(downto + 1, selected_num - 1, excluded_num);
			downto = selected_num;
		}
	}

	draw_score();
	draw_updown();
	draw_info();

	update_panels();
	doupdate();
}

void update_game()
{
	if (is_init)
	{
		game_settup();
	}
	else
	{
		++score;
		
		if (selected_num == ans)
		{
			is_correct = true;
			game_over = true;
			if (score < best_score)
				best_score = score;
			return;
		}
	
		is_correct = false;
		if (selected_num < ans)
			is_greater = false;
		else
			is_greater = true;
	}

}

bool get_selected_num(MEVENT &mevent)
{
	int x, y, num;
	int offset_x_ingame = 9, offset_y_ingame = 4;

	mevent.x -= offset_x_ingame;
	mevent.y -= offset_y_ingame;
	
	// 공백 클릭은 무시한다
	if (mevent.x < 0 || mevent.x >= 30 || mevent.y < 0 || mevent.y > 10 || mevent.x % 3 == 2)
		return false;

	x = mevent.x / (CHAR_WIDTH + CHAR_MARGIN);
	y = mevent.y * 10;
	num = x + y;

	// 제외된 범위 클릭은 무시한다
	if (num <= downto || num >= upto)
		return false;
	
	selected_num = num;
	return true;
}

void run()
{
	while (true)
	{
		// 입력 대기
		int c = getch();
	
		// 재시작
		if (c == 'r')
		{
			game_settup();
			draw_game();
			is_init = false;
			continue;
		}
		// 종료
		if (c == 'q')
		{
			return;
		}
		// 게임 오버시 그 외 입력은 무시
		if (game_over == true)
			continue;
		
		if (c == KEY_MOUSE)
		{
			MEVENT mevent;
	
			if (getmouse(&mevent) == OK)
			{
				if (mevent.bstate & BUTTON1_CLICKED)
				{
					if (get_selected_num(mevent) == false)
						continue;
					update_game();
					draw_game();
				}
			}
		}
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
	game_over = false;
	is_greater = false;
	is_correct = false;
	is_init = true;
}

void create_ingame_panels()
{
	// ingame contents
	{
		int offset_x_ingame = 6, offset_y_ingame = 1;
	
		// windows
		w_background = newwin(15, 54, 0, 0);
		w_number = newwin(12, 33, 0, 0);
		w_score = newwin(4, 20, 0, 0);
		w_updown = newwin(2, 8, 0, 0);
		w_info = newwin(6, 20, 0, 0);
	
		// panels
		p_background = new_panel(w_background);
		p_number = new_panel(w_number);
		p_score = new_panel(w_score);
		p_updown = new_panel(w_updown);
		p_info = new_panel(w_info);
	
		// background
		apply_format(w_background, highlight_num);
		mvwprintw(w_background, 0, 0, "Up and Down");
		mvwprintw(w_background, 0, 41, "by. YHY_GAMES");
		remove_format(w_background, highlight_num);
		mvwprintw(w_background, 1, 0, "──────────────────────────────────────────────────────");
		mvwprintw(w_background, 14, 0, "──────────────────────────────────────────────────────");
		move_panel(p_background, offset_y_ingame, offset_x_ingame);
	
		// number board
		box(w_number, 0, 0);
		move_panel(p_number, 2 + offset_y_ingame, offset_x_ingame);
	
		// score board
		box(w_score, 0, 0);
		move_panel(p_score, 2 + offset_y_ingame, 34 + offset_x_ingame);
	
		// up down
		mvwprintw(w_updown, 0, 3, "UP");
		mvwprintw(w_updown, 1, 2, "DOWN");
		move_panel(p_updown, 6 + offset_y_ingame, 40 + offset_x_ingame);
	
		// info board
		box(w_info, 0, 0);
		move_panel(p_info, 8 + offset_y_ingame, 34 + offset_x_ingame);
	}

	update_panels();
	doupdate();
}

void title()
{
	// title
	WINDOW *w_title;
	PANEL *p_title;

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

	update_panels();
	doupdate();

	getch();

	del_panel(p_title);
	werase(w_title);
	delwin(w_title);

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
	init_pair(1, COLOR_WHITE, COLOR_BLACK); // Normal Number
	init_pair(2, 8, COLOR_BLACK); // Exclude Number
	init_pair(3, COLOR_RED, COLOR_BLACK); // Wrong Number
	init_pair(4, COLOR_GREEN, COLOR_BLACK); // Correct Number
	init_pair(5, COLOR_YELLOW, COLOR_BLACK); // Highlight Number

	refresh();

	// canvas
	{
		w_canvas = newwin(MAXHEIGHT, MAXWIDTH, 0, 0);
		p_canvas = new_panel(w_canvas);
		box(w_canvas, 0, 0);
	}
}

void test()
{
	//
}

int main()
{
	// 초기화를 진행합니다.
	init();

	// 타이틀 표시
	title();
	
	// 인게임 패널들 생성
	create_ingame_panels();

	// 게임 초기화
	game_settup();
	
	// 게임화면 표시
	draw_game();
	is_init = false;

	run();

	endwin();
}