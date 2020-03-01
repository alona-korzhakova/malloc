#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	printf("%d", getpagesize());
	return 0;
}
