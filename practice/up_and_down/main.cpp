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
	for (int i = 0; i < 10; i++)
	{
		mvprintw(i, 0, "■");
	}
	for (int i = 0; i < 10; i++)
	{
		mvprintw(0, 2 * i, "■");
	}
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