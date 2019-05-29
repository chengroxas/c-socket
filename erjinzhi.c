#include <stdio.h>
#include <stdlib.h>

void digui(int number);
int main()
{
	digui(252);
}

void digui(int number)
{
	if (number <= 1)
	{
		printf("%d",number);
	}
	else 
	{
		digui(number / 2);
		printf("%d", number % 2);
	}
}
