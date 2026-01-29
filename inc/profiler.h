#ifndef PROFILER_H
#define PROFILER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

// Macro to track variable assignments
#define TRACK_ASSIGN(var, value) do { \
	(var) = (value); \
	send_assignment_event(#var, (void*)(var)); \
} while(0)

typedef struct s_memory
{
	char*	memory;
	size_t	alignment;
}			t_memory;

int		init_pipe(void);
pid_t	init_display_process(void);
pid_t	init_binary_process(char* argv[]);
void	init_parent_process(char* argv, pid_t display_pid, pid_t binary_pid, int pipe_fd);
void	send_assignment_event(const char* var_name, const void* var_value);

#endif