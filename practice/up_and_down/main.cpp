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

int ans, upto, downto;

void init();

void draw_nums(int from, int to, attr_t *attr, int *color)
{
	char buf[BUFMAX];
	
	if (attr != NULL)
		attron(*attr);
	if (color != NULL)
		attron(COLOR_PAIR(*color));
	
	for (int num = from; num <= to; num++)
	{
		int x, y;

		x = num % 10;
		y = num / 10;
		snprintf(buf, BUFMAX, "%.2d", num);
		mvprintw(y, x * (CHAR_WIDTH + CHAR_MARGIN), buf);
	}

	if (attr != NULL)
		attroff(*attr);
	if (color != NULL)
		attroff(COLOR_PAIR(*color));
}

void draw2()
{
	char buf[BUFMAX];

	// downto 비활성화 부분
	attron(A_DIM);
	attron(COLOR_PAIR(2));
	for (int num = 0; num < downto; num++)
	{
		int x, y;

		x = num % 10;
		y = num / 10;
		snprintf(buf, BUFMAX, "%.2d", num);
		mvprintw(y, x * (CHAR_WIDTH + CHAR_MARGIN), buf);
	}
	attroff(A_DIM);
	attroff(COLOR_PAIR(2));

	// 선택한 부분
	attron(COLOR_PAIR(3));
	if (downto >= 0)
	{
		int x, y;

		x = downto % 10;
		y = downto / 10;
		snprintf(buf, BUFMAX, "%.2d", downto);
		mvprintw(y, x * (CHAR_WIDTH + CHAR_MARGIN), buf);
	}
	attroff(COLOR_PAIR(3));

	// 활성화 부분
	attron(COLOR_PAIR(1));
	for (int num = downto + 1; num < upto; num++)
	{
		int x, y;

		x = num % 10;
		y = num / 10;
		snprintf(buf, BUFMAX, "%.2d", num);
		mvprintw(y, x * (CHAR_WIDTH + CHAR_MARGIN), buf);
	}
	attroff(COLOR_PAIR(1));

	// 선택한 부분
	attron(COLOR_PAIR(3));
	if (upto <= 99)
	{
		int x, y;

		x = upto % 10;
		y = upto / 10;
		snprintf(buf, BUFMAX, "%.2d", upto);
		mvprintw(y, x * (CHAR_WIDTH + CHAR_MARGIN), buf);
	}
	attroff(COLOR_PAIR(3));

	// upto 비활성화 부분
	attron(A_DIM);
	attron(COLOR_PAIR(2));
	for (int num = upto + 1; num < 100; num++)
	{
		int x, y;

		x = num % 10;
		y = num / 10;
		snprintf(buf, BUFMAX, "%.2d", num);
		mvprintw(y, x * (CHAR_WIDTH + CHAR_MARGIN), buf);
	}
	attroff(A_DIM);
	attroff(COLOR_PAIR(2));

	refresh();
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

	snprintf(buf, BUFMAX, "mouse event detected! x: %d, y: %d", mevent.x, mevent.y);
	mvprintw(11, 0, buf);

	snprintf(buf, BUFMAX, "clicked number: %.2d", num);
	mvprintw(12, 0, buf);

	if (num < ans)
	{
		snprintf(buf, BUFMAX, "%.2d is smaller than answer", num);
		mvprintw(13, 0, buf);
		downto = num;
		draw2();
	}
	else if (num > ans)
	{
		snprintf(buf, BUFMAX, "%.2d is greater than answer", num);
		mvprintw(13, 0, buf);
		upto = num;
		draw2();
	}
	else
	{
		clear();
		snprintf(buf, BUFMAX, "Game Over");
		mvprintw(0, 0, buf);
		snprintf(buf, BUFMAX, "%.2d is answer              ", num);
		mvprintw(13, 0, buf);
	}

	// draw2();
}

void draw()
{
	attron(COLOR_PAIR(1));
	for (int y = 0; y < 10; y++)
	{
		for (int x = 0; x < 10; x++)
		{
			std::stringstream ss;
			ss.width(2);
			ss << (y * 10) + x;
			
			mvprintw(y, x * (CHAR_WIDTH + CHAR_MARGIN), ss.str().c_str());
		}
	}
	attroff(COLOR_PAIR(1));
	refresh();
}

void run()
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
			init();
			draw2();
			break;
	}

	// 나중에 매번 draw 하는것이 아니라 한 번이라도 유효한 입력이 있으면 draw 하도록 수정하기
	// draw();
}

void init()
{
	// 0 ~ 99 사이의 무작위 숫자를 생성합니다.
	srand(time(0));
	ans = rand() % 100 + 1;

	// 재시작을 위해 upto, downto 초기화를 init에서 수행
	upto = DEFAULT_UPTO;
	downto = DEFAULT_DOWNTO;

	// ncurses 의 초기화 함수들을 설정합니다.
	setlocale(LC_CTYPE, "");
	initscr();
	curs_set(0);
	noecho();
	keypad(stdscr, TRUE);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	init_pair(3, COLOR_BLACK, COLOR_RED);
}

int main()
{
	// 초기화를 진행합니다.
	init();
	draw2();

	// 계속해서 반복되는 핵심 부분입니다.
	while(1)
	{
		// 화면을 렌더링합니다.
		run();
	}

	// ncurses 모드를 종료합니다.
	endwin();
}