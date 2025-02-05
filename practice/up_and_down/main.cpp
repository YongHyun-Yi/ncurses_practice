#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>

#include <sstream>

int main()
{
	int ans;
	srand(time(0));
	ans = rand() % 100 + 1;

	std::cout << "answer: " << ans << "\n\n";
	while (1)
	{
		std::string str;
		std::stringstream ss;
		int input;

		std::cout << "guessing: ";
		std::cin >> str;
		ss << str;
		ss >> input;

		if (input == ans)
		{
			std::cout << "\ncorrect!\n";
			break ;
		}
		std::cout << "wrong...try again!\n\n";
	}
}