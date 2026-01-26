#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <malloc.h>

// Function pointers to the real malloc/free
static void* (*real_malloc)(size_t) = NULL;
static void (*real_free)(void*) = NULL;
static void* (*real_calloc)(size_t, size_t) = NULL;

// Bootstrap buffer for dlsym's own malloc calls
#define BOOTSTRAP_SIZE 1024
static char bootstrap_buffer[BOOTSTRAP_SIZE];
static size_t bootstrap_offset = 0;
static int initializing = 0;

static int profiler_fd = -1;

static void send_alloc_event(const char* msg, size_t len) {
    if (profiler_fd < 0) {
        const char* pipe_path = getenv("MEMPROF_PIPE");
        if (pipe_path) {
            profiler_fd = open(pipe_path, O_WRONLY | O_NONBLOCK);
        }
    }
    if (profiler_fd >= 0) {
        write(profiler_fd, msg, len);
    }
}

// Get pointers to the real functions (first time called)
static void init_hooks(void) {
    if (real_malloc)
        return;
    
    initializing = 1;
    real_malloc = (void* (*)(size_t)) dlsym(RTLD_NEXT, "malloc");
    real_free = (void (*)(void*)) dlsym(RTLD_NEXT, "free");
    real_calloc = (void* (*)(size_t, size_t)) dlsym(RTLD_NEXT, "calloc");
    initializing = 0;
}

// Wrap malloc
void* malloc(size_t size) {
    // During initialization, use bootstrap buffer
    if (initializing) {
        void* ptr = &bootstrap_buffer[bootstrap_offset];
        bootstrap_offset += size;
        if (bootstrap_offset >= BOOTSTRAP_SIZE) {
            // char buf[64];
            // int len = snprintf(buf, sizeof(buf), "Bootstrap buffer overflow!\n");
            // if (len > 0)
            //    write(STDERR_FILENO, buf, (size_t)len);
            return NULL;
        }
        return ptr;
    }
    
    init_hooks();
    void* ptr = real_malloc(size);
    
    // Send allocation event to profiler with usable size
    size_t usable = malloc_usable_size(ptr);
    char buf[160];
    int len = snprintf(buf, sizeof(buf), "[ALLOC] size=%zu usable=%zu ptr=%p\n", size, usable, ptr);
    if (len > 0)
        send_alloc_event(buf, (size_t)len);
    
    return ptr;
}

// Wrap calloc (dlsym uses calloc internally)
void* calloc(size_t nmemb, size_t size) {
    // During initialization, use bootstrap buffer
    if (initializing) {
        void* ptr = &bootstrap_buffer[bootstrap_offset];
        size_t total = nmemb * size;
        memset(ptr, 0, total);
        bootstrap_offset += total;
        if (bootstrap_offset >= BOOTSTRAP_SIZE) {
            // char buf[64];
            // int len = snprintf(buf, sizeof(buf), "Bootstrap buffer overflow!\n");
            // if (len > 0) write(STDERR_FILENO, buf, (size_t)len);
            return NULL;
        }
        return ptr;
    }
    
    init_hooks();
    void* ptr = real_calloc(nmemb, size);
    
    // Send allocation event to profiler with usable size
    size_t usable = malloc_usable_size(ptr);
    char buf[160];
    int len = snprintf(buf, sizeof(buf), "[CALLOC] nmemb=%zu size=%zu usable=%zu ptr=%p\n", nmemb, size, usable, ptr);
    if (len > 0)
        send_alloc_event(buf, (size_t)len);
    
    return ptr;
}

// Wrap free
void free(void* ptr) {
    // Ignore frees from bootstrap buffer
    if (ptr >= (void*)bootstrap_buffer && 
        ptr < (void*)(bootstrap_buffer + BOOTSTRAP_SIZE)) {
        return;
    }
    
    init_hooks();
    
    // Send free event to profiler
    char buf[64];
    int len = snprintf(buf, sizeof(buf), "[FREE] ptr=%p\n", ptr);
    if (len > 0)
        send_alloc_event(buf, (size_t)len);
    
    real_free(ptr);
}