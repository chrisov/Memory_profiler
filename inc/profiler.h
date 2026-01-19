#ifndef PROFILER_H
#define PROFILER_H

#include <unistd.h>
#include <stdio.h>

long	PAGE_SIZE;

typedef struct s_memory
{
	char*	memory;
	size_t	alignment;
}			t_memory;

#endif