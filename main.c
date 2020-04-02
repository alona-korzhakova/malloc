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
	while (i < 1)//1024)
	{
		s_calloc(500, sizeof(t_block));
		s_malloc(7000000);
		s_malloc(7);


		printf("MALLOC 0\n");
		if ((addr = (char *)s_malloc(200)) == NULL) {
			printf("Not ok...malloc\n");
			return (0);
		}
		addr[0] = 'h';
		addr[1] = 'e';
		addr[2] = 'l';
		addr[3] = 'l';
		addr[4] = 'o';
		addr[5] = '+';
		addr[20] = 'b';
		addr[198] = 'c';
		show_alloc_mem();		
		// //addr[1998] = 42;
		// printf("REALLOC 0\n");
		// if ((addr = (char *)s_malloc(600)) == NULL) {
		// 	printf("Not ok...malloc\n");
		// 	return (0);
		// }
		// printf("FREE 0\n");
		// s_free(addr);
		// printf("MALLOC 1\n");
		// if ((addr = (char *)s_malloc(200)) == NULL) {
		// 	printf("Not ok...malloc\n");
		// 	return (0);
		// }
		// printf("MALLOC 2\n");		
		// if ((addr1 = (char *)s_malloc(200)) == NULL) {
		// 	printf("Not ok...malloc\n");
		// 	return (0);
		// }
		// printf("MALLOC 3\n");		
		// if ((addr2 = (char *)s_malloc(200)) == NULL) {
		// 	printf("Not ok...malloc\n");
		// 	return (0);
		// }
		// printf("MALLOC 4\n");		
		// if ((addr3 = (char *)s_malloc(200)) == NULL) {
		// 	printf("Not ok...malloc\n");
		// 	return (0);
		// }
		// printf("MALLOC 5\n");		
		// if ((addr4 = (char *)s_malloc(200)) == NULL) {
		// 	printf("Not ok...malloc\n");
		// 	return (0);
		// }
		// printf("FREE 1\n");	
		// s_free(addr);
		// printf("FREE 2\n");
		// s_free(addr1);
		// printf("FREE 3\n");		
		// s_free(addr2);
		// printf("FREE 4\n");		
		// s_free(addr3);
		// printf("FREE 5\n");		
		// s_free(addr4);
		// printf("FREE double 5\n");		
		// s_free(addr4);
		// check();
		// if ((addr = (char *)s_malloc(40000)) == NULL) {
		// 	printf("Not ok...malloc\n");
		// 	return (0);
		// }
		//printf("return address realloc is: %p\n", (void *)addr);
		//s_free(addr);
		i++;
		//printf("it's ok\n");
	}
	// //check();
	return (0);
}