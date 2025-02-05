#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>

int main()
{
	int ans;
	srand(time(0));
	ans = rand() % 100 + 1;

	std::cout << "answer: " << ans << "\n";
}