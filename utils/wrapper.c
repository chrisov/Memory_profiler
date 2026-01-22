#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <unistd.h>

// Function pointers to the real malloc/free
static void* (*real_malloc)(size_t) = NULL;
static void (*real_free)(void*) = NULL;
static void* (*real_calloc)(size_t, size_t) = NULL;

// Bootstrap buffer for dlsym's own malloc calls
#define BOOTSTRAP_SIZE 1024
static char bootstrap_buffer[BOOTSTRAP_SIZE];
static size_t bootstrap_offset = 0;
static int initializing = 0;

// Get pointers to the real functions (first time called)
static void init_hooks(void) {
    if (real_malloc) return;
    
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
            // if (len > 0) write(STDERR_FILENO, buf, (size_t)len);
            return NULL;
        }
        return ptr;
    }
    
    init_hooks();
    void* ptr = real_malloc(size);
    // char buf[128];
    // int len = snprintf(buf, sizeof(buf), "[ALLOC] size=%zu, ptr=%p\n", size, ptr);
    // if (len > 0) write(STDERR_FILENO, buf, (size_t)len);
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
    // char buf[128];
    // int len = snprintf(buf, sizeof(buf), "[CALLOC] nmemb=%zu, size=%zu, ptr=%p\n", nmemb, size, ptr);
    // if (len > 0) write(STDERR_FILENO, buf, (size_t)len);
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
    // char buf[64];
    // int len = snprintf(buf, sizeof(buf), "[FREE] ptr=%p\n", ptr);
    // if (len > 0) write(STDERR_FILENO, buf, (size_t)len);
    real_free(ptr);
}