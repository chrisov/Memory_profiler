#ifndef PROFILER_H
#define PROFILER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

// long	PAGE_SIZE;

typedef struct s_memory
{
	char*	memory;
	size_t	alignment;
}			t_memory;

int		init_pipe(void);
pid_t	init_display_process(void);
pid_t	init_binary_process(char* argv[]);
void	init_parent_process(char* argv, pid_t display_pid, pid_t binary_pid, int pipe_fd);

#endif