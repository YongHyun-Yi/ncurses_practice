#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include <locale.h>
#include <ncurses.h>
#include <panel.h>

#include <sstream>

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
// 선택한 숫자가
// 1. 정답보다 작다
// 2. 정답보다 크다
// 3. 정답과 같다
// 셋 중에 한 가지 상태만 가능함으로
// 해당 범위만 해당 서식으로 그려준다
// 이전 범위를 중복해서 그릴 필요가 없어지고
// 원래 기획 의도대로 이전에 선택한 오답 표시도 남는다
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
	std::stringstream ss;
	char buf[BUFMAX];
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
	// 0 ~ 99 사이의 무작위 숫자를 생성합니다.
	srand(time(0));
	ans = rand() % 100 + 1;

	// ncurses 의 초기화 함수들을 설정합니다.
	setlocale(LC_CTYPE, "");
	initscr();
	curs_set(0);
	noecho();
	keypad(stdscr, TRUE);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

	start_color();
	init_color(8, 500, 500, 500); // A_DIM 대신 회색을 추가한다
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, 8, COLOR_BLACK);
	init_pair(3, COLOR_BLACK, COLOR_RED);
}

void draw_title()
{
	mvaddstr(0, 0,
		"   __  __         ___              __   ____                    \n"
		"  / / / /___     /   |  ____  ____/ /  / __ \\____ _      ______ \n"
		" / / / / __ \\   / /| | / __ \\/ __  /  / / / / __ \\ | /| / / __ \\\n"
		"/ /_/ / /_/ /  / ___ |/ / / / /_/ /  / /_/ / /_/ / |/ |/ / / / /\n"
		"\\____/ .___/  /_/  |_/_/ /_/\\__,_/  /_____/\\____/|__/|__/_/ /_/ \n"
		"    /_/                                                         \n"
		"                      Press AnyKey to Start\n"
	);
	refresh();
}

int main()
{
	// 초기화를 진행합니다.
	init();

	// 타이틀 화면 표시
	draw_title();

	getch();

	// 게임화면 표시
	clear();
	game_settup();
	draw_game();

	// 계속해서 반복되는 핵심 부분입니다.
	run();

	// ncurses 모드를 종료합니다.
	endwin();
}