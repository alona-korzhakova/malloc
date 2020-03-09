#include "ft_malloc.h"

int main()
{
	int i;
	char *addr;

	i = 0;
	while (i < 500)//1024)
	{
		if ((addr = (char *)s_malloc(500)) == NULL) {
			printf("Not ok...\n");
			return (0);
		}
		
		addr[1049] = 42;
		//printf("OK, i: %d\n", i);
		i++;
		//printf("it's ok\n");
	}
	check();
	return (0);
}