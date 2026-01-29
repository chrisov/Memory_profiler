#include "../inc/profiler.h"

// The Hierarchy:
//     The OS gives a Page (4 KB) to the program.
//     The Allocator (malloc) carves that Page into many Chunks.
//     Each Chunk is sized and aligned to a 16-byte Alignment Unit.

int main (int argc, char* argv[]) {
	pid_t	display_pid, binary_pid;
	int		pipe_fd;

	if (argc < 2)
		return (fprintf(stderr, "Usage: %s <program> [args...]\n", argv[0]), 1);
	display_pid = init_display_process();
	binary_pid = init_binary_process(argv);
	pipe_fd = init_pipe();
	init_parent_process(argv[1], display_pid, binary_pid, pipe_fd);
	
	return (0);
}
