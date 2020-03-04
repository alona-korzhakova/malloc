#include "ft_malloc.h"

int main()
{
	int i;
	char *addr;

	i = 0;
	while (i < 1)//1024)
	{
		if ((addr = (char *)s_malloc(1024)) == NULL) {
			printf("Not ok...\n");
			return (0);
		}
		addr[0] = 42;
		i++;
		printf("it's ok\n");
	}
	return (0);
}