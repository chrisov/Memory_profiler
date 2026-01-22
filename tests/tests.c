#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

int	main(void) {
	void	*p1;
	void	*p2;
	void	*p3;
	void	*p4;

	printf("================== Test 1 ==================");
	printf("\nSimple tracking of consecutive memory chunks.\n");
	{
		const int SIZE1 = 1;
		const int SIZE2 = 10;
		const int SIZE3 = 24;
		const int SIZE4 = 50;

		// exec(clear);

		// Minimum memory chunk in 64-bit architectures: 32 bytes
		p1 = malloc(SIZE1);
		printf("\n---- Minimum chunk size ----\n");
		printf("Pointer p1: %p\n", p1);
		printf("Requested chunk size: %d\n", SIZE1);
		printf("Actual chuck size: %zu\n", malloc_usable_size(p1));
		for (int i = 0; i <= SIZE1 + 5; i++) {
			((char*)p1)[i] = 'A' + i;
		}
		printf("Written data (%ld): '", strlen((char*)p1));
		for (int i = 0; i <= SIZE1 + 5; i++) {
			printf("%c", ((char*)p1)[i]);
		}
		printf("'\n");
		
		// Can write more than 10 bytes, because of 16-byte alignment
		p2 = malloc(SIZE2);
		printf("\n---- 16-byte alignment ----\n");
		printf("Pointer p2: %p\n", p2);
		printf("Requested chunk size: %d\n", SIZE2);
		printf("Actual chuck size: %zu\n", malloc_usable_size(p2));
		for (int i = 0; i <= SIZE2 + 5; i++) {
			((char*)p2)[i] = 'A' + i;
		}
		printf("Written data (%ld): '", strlen((char*)p2));
		for (int i = 0; i <= SIZE2 + 5; i++) {
			printf("%c", ((char*)p2)[i]);
		}
		printf("'\n");
		
		// Writing beyond the chunk's size will result messing with the next chunk's metadata
		// and abort the allocation entirely.
		p3 = malloc(SIZE3);
		printf("\n---- Exact alignment with memory chunk ----\n");
		printf("Pointer p3: %p\n", p3);
		printf("Requested chunk size: %d\n", SIZE3);
		printf("Actual chuck size: %zu\n", malloc_usable_size(p3));
		for (int i = 0; i <= SIZE3; i++) {
			((char*)p3)[i] = 'A' + i;
		}
		printf("Written data (%ld): '", strlen((char*)p3));
		for (int i = 0; i < SIZE3 + 1; i++) {
			printf("%c", ((char*)p3)[i]);
		}
		printf("'\n");

		// Allocated chunk with no user data
		p4 = malloc(SIZE4);

		free(p1);
		free(p2);
		free(p3);
		free(p4);
	}
	printf("\n================ End of Test 1 ==============\n");

}