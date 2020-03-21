#include "ft_malloc.h"

int main()
{
	int i;
	char *addr;
	char *addr1;
	char *addr2;
	char *addr3;
	char *addr4;
	// int d = 2;
	// for (i = 0; i < 2; i++) {
	// 	int *ptr = (int *)malloc(d);

	// 	printf("return malloc address is %p\n", ptr);
	// }
	//printf("sizeof long double: %ld\n", sizeof(long double));
	i = 0;
	while (i < 2)//1024)
	{
		printf("================= NUM OF ITERATION IS: %d ================\n", i);
		if ((addr = (char *)s_malloc(200)) == NULL) {
			printf("Not ok...malloc\n");
			return (0);
		}
		printf("return address malloc is: %p\n", (void *)addr);		
		//addr[1998] = 42;

		if ((addr = (char *)s_realloc(addr, 200)) == NULL) {
			printf("Not ok...realloc\n");
			return (0);
		}

		if ((addr = (char *)s_realloc(addr, 200)) == NULL) {
			printf("Not ok...realloc\n");
			return (0);
		}
		s_free(addr);

		if ((addr = (char *)s_malloc(200)) == NULL) {
			printf("Not ok...malloc\n");
			return (0);
		}
		if ((addr1 = (char *)s_malloc(200)) == NULL) {
			printf("Not ok...malloc\n");
			return (0);
		}
		if ((addr2 = (char *)s_malloc(200)) == NULL) {
			printf("Not ok...malloc\n");
			return (0);
		}
		if ((addr3 = (char *)s_malloc(200)) == NULL) {
			printf("Not ok...malloc\n");
			return (0);
		}
		if ((addr4 = (char *)s_malloc(200)) == NULL) {
			printf("Not ok...malloc\n");
			return (0);
		}
		s_free(addr);
		s_free(addr1);
		s_free(addr2);
		s_free(addr3);
		s_free(addr4);
		s_free(addr4);
		// if ((addr = (char *)s_malloc(40000)) == NULL) {
		// 	printf("Not ok...malloc\n");
		// 	return (0);
		// }
		//printf("return address realloc is: %p\n", (void *)addr);
		//s_free(addr);
		i++;
		printf("===================== END OF ITERATION ==================\n");
		//printf("it's ok\n");
	}
	// //check();
	return (0);
}