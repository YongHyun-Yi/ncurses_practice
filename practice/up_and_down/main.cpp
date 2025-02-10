#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include <locale.h>
#include <ncurses.h>
#include <panel.h>

#include <sstream>

int ans, upto = 101, downto = 0;

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
			
			mvprintw(y, x * 3, ss.str().c_str());
		}
	}
	attroff(COLOR_PAIR(1));
	refresh();

	// 마우스 입력 받기
	int c = getch();
	MEVENT mevent;
	std::stringstream ss;
	switch(c)
	{
		case KEY_MOUSE:
			if (getmouse(&mevent) == OK)
			{
				ss << "mouse event detected! x: " << std::to_string(mevent.x) << " y: " << std::to_string(mevent.y);
				mvprintw(11, 0, ss.str().c_str());
				ss.str("");
				break ;
			}
	}
	refresh();
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

	// 계속해서 반복되는 핵심 부분입니다.
	while(1)
	{
		// 화면을 렌더링합니다.
		draw();
	}

	// ncurses 모드를 종료합니다.
	endwin();
}