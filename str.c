#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char name[10];
	fgets(name, 11, stdin);
	printf("%s", name);
}
