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

int ans, upto = 101, downto = 0;

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
	}

	// 나중에 매번 draw 하는것이 아니라 한 번이라도 유효한 입력이 있으면 draw 하도록 수정하기
	draw();
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
	keypad(stdscr, TRUE);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
}

int main()
{
	// 초기화를 진행합니다.
	init();
	draw();

	// 계속해서 반복되는 핵심 부분입니다.
	while(1)
	{
		// 화면을 렌더링합니다.
		run();
	}

	// ncurses 모드를 종료합니다.
	endwin();
}