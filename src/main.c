#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

// The Hierarchy:
//     The OS gives a Page (4 KB) to the program.
//     The Allocator (malloc) carves that Page into many Chunks.
//     Each Chunk is sized and aligned to a 16-byte Alignment Unit.


// Define the structure of the metadata
struct chunk_metadata {
    size_t prev_size;
    size_t size_field; // Includes flags in the last 3 bits
};

// The Union: The heart of your visualizer
union HeapChunk {
    struct chunk_metadata header; // The "Metadata" view
    char raw_bytes[1];            // The "Raw Memory" view
};

// Helper macros to clean up the size_field
#define GET_SIZE(s) ((s) & ~7)
#define PREV_INUSE(s) ((s) & 1)

void inspect_chunk(void* user_ptr) {
    // malloc returns a pointer to the DATA, so we go back 16 bytes
    union HeapChunk* chunk = (union HeapChunk*)((char*)user_ptr - 16);

    size_t raw_size = GET_SIZE(chunk->header.size_field);
    int in_use = PREV_INUSE(chunk->header.size_field);

    printf("--- Chunk at %p ---\n", (void*)chunk);
    printf("Header Size Field: %zu\n", chunk->header.size_field);
    printf("Actual Data Size:  %zu bytes\n", raw_size);
    printf("Status:            %s\n", in_use ? "Allocated" : "Free");
    
    // Visualize the first 8 bytes of "data"
    printf("First 8 bytes of data: ");
    for(int i = 0; i < 8; i++) {
        // We skip the 16 bytes of header to see user data
        printf("%02x ", (unsigned char)chunk->raw_bytes[16 + i]);
    }
    printf("\n\n");
}

int main() {
    void* p1 = malloc(20);
    sprintf((char*)p1, "Hello!"); // Put some data in it

    inspect_chunk(p1);

    free(p1);
    return 0;
}