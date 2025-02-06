#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include <locale.h>
#include <ncurses.h>
#include <panel.h>

#include <sstream>

void draw()
{
	setlocale(LC_CTYPE, "");
	initscr();
	curs_set(0);
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_WHITE);
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
	getch();
	endwin();
}

int main()
{
	// 1 ~ 100
	int ans, upto = 101, downto = 0;
	srand(time(0));
	ans = rand() % 100 + 1;
	draw();

	// std::cout << "answer: " << ans << "\n\n";
	// while (1)
	// {
	// 	std::string str;
	// 	std::stringstream ss;
	// 	int input;

	// 	std::cout << "guessing: ";
	// 	std::cin >> str;
	// 	ss << str;
	// 	ss >> input;

	// 	if (input == ans)
	// 	{
	// 		std::cout << "\ncorrect!\n";
	// 		break ;
	// 	}
	// 	if (input <= downto || input >= upto)
	// 	{
	// 		std::cout << "that number was already excluded! try again!\n\n";
	// 	}
	// 	else
	// 	{
	// 		if (input > ans)
	// 		{
	// 			upto = input;
	// 		}
	// 		else
	// 		{
	// 			downto = input;
	// 		}
	// 		std::cout << "wrong...try again!\n\n";
	// 	}
	// }
}