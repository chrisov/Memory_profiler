#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

static int profiler_fd = -1;

// Track variable assignments (no malloc wrapping)
void send_assignment_event(const char* var_name, const void* var_value) {
    if (profiler_fd < 0) {
        const char* pipe_path = getenv("MEMPROF_PIPE");
        if (pipe_path) {
            // Try non-blocking first, then blocking if that fails
            profiler_fd = open(pipe_path, O_WRONLY | O_NONBLOCK);
            if (profiler_fd < 0) {
                profiler_fd = open(pipe_path, O_WRONLY);
            }
        }
    }
    if (profiler_fd >= 0) {
        char buf[256];
        int len = snprintf(buf, sizeof(buf), "[ASSIGN] var=%s value=%p\n", 
            var_name, var_value);
        if (len > 0)
            write(profiler_fd, buf, (size_t)len);
    }
}
