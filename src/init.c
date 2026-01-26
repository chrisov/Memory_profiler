#include "../inc/profiler.h"

pid_t	init_display_process(void) {
	// Create named pipe for display
	unlink("/tmp/memprof_pipe");
	mkfifo("/tmp/memprof_pipe", 0666);

	// Fork #1: Display terminal
	pid_t display_pid = fork();
	if (display_pid == 0) {
		execlp("xterm", "xterm", "-e", "sh", "-c", 
				"echo 'Memory Profiler Display'; cat /tmp/memprof_pipe; echo ''; echo 'Press Enter to exit...'; read dummy </dev/tty", NULL);
		execlp("gnome-terminal", "gnome-terminal", "--", "sh", "-c",
				"echo 'Memory Profiler Display'; cat /tmp/memprof_pipe; echo ''; echo 'Press Enter to exit...'; read dummy </dev/tty", NULL);
		_exit(1);
	}

	// Wait for display terminal to start
	sleep(1);
	return (display_pid);
}

int	init_pipe(void) {
	// Open display pipe
	int pipe_fd = open("/tmp/memprof_pipe", O_WRONLY | O_NONBLOCK);
	if (pipe_fd < 0) {
		pipe_fd = open("/tmp/memprof_pipe", O_WRONLY);
	}
	if (pipe_fd >= 0) {
		int flags = fcntl(pipe_fd, F_GETFL);
		fcntl(pipe_fd, F_SETFL, flags & ~O_NONBLOCK);
	}
	return (pipe_fd);
}

pid_t	init_binary_process(char *argv[]) {
	pid_t test_pid = fork();
	if (test_pid == 0) {
		// Child: run the test program
		setenv("MEMPROF_PIPE", "/tmp/memprof_pipe", 1);
		setenv("LD_PRELOAD", "./utils/wrapper.so", 1);
		execvp(argv[1], &argv[1]); // argv[1] = program, argv[2...] = its args
		perror("execvp failed");
		_exit(127);
	}
	return (test_pid);
}

void	init_parent_process(char* argv, pid_t display_pid, pid_t binary_pid, int pipe_fd) {
	char buf[256];
	int len = snprintf(buf, sizeof(buf), "Executing '%s' (PID %d)\n", argv, binary_pid);
	if (pipe_fd >= 0)
		write(pipe_fd, buf, len);

	// Wait for test program to complete
	int status;
	waitpid(binary_pid, &status, 0);
	
	len = snprintf(buf, sizeof(buf), "\nTest program exited with status %d\n", 
				   WIFEXITED(status) ? WEXITSTATUS(status) : -1);
	if (pipe_fd >= 0)
		write(pipe_fd, buf, len);

	// Close pipe so cat gets EOF in display terminal
	if (pipe_fd >= 0)
		close(pipe_fd);
	
	// Wait for user to close display terminal
	waitpid(display_pid, NULL, 0);
	
	unlink("/tmp/memprof_pipe");
}
