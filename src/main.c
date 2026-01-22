#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

// The Hierarchy:
//     The OS gives a Page (4 KB) to the program.
//     The Allocator (malloc) carves that Page into many Chunks.
//     Each Chunk is sized and aligned to a 16-byte Alignment Unit.


// Define the structure of the metadata
// struct chunk_metadata {
//     size_t prev_size;
//     size_t size_field; // Includes flags in the last 3 bits
// };

// // The Union: The heart of your visualizer
// union HeapChunk {
//     struct chunk_metadata header; // The "Metadata" view
//     char raw_bytes[1];            // The "Raw Memory" view
// };

// // Helper macros to clean up the size_field
// #define GET_SIZE(s) ((s) & ~7)
// #define PREV_INUSE(s) ((s) & 1)

// void inspect_chunk(void* user_ptr) {
//     // malloc returns a pointer to the DATA, so we go back 16 bytes
//     union HeapChunk* chunk = (union HeapChunk*)((char*)user_ptr - 16);

//     size_t raw_size = GET_SIZE(chunk->header.size_field);
//     int in_use = PREV_INUSE(chunk->header.size_field);

//     printf("--- Chunk at %p ---\n", (void*)chunk);
//     printf("User pointer at: %p\n", user_ptr);
    
//     // Display header contents
//     printf("\nHeader (16 bytes):\n");
//     printf("  prev_size:   %zu (0x%016lx)\n", chunk->header.prev_size, chunk->header.prev_size);
//     printf("  size_field:  %zu (0x%016lx)\n", chunk->header.size_field, chunk->header.size_field);
//     printf("  Raw bytes:   ");
//     for(int i = 0; i < 16; i++) {
//         printf("%02x ", (unsigned char)chunk->raw_bytes[i]);
//         if(i == 7) printf("| "); // Separator between prev_size and size_field
//     }
//     printf("\n");
    
//     printf("\nParsed values:\n");
//     printf("  Total Chunk Size: %zu bytes (entire chunk from start to start of next)\n", raw_size);
//     printf("  size_field metadata: 8 bytes\n");
//     printf("  Usable by user: %zu bytes (if prev_size is available)\n", raw_size - 8);
//     printf("  Status:            %s\n", in_use ? "Allocated" : "Free");
    
//     // Visualize the first 8 bytes of "data"
//     printf("\nFirst 8 bytes of user data: ");
//     for(int i = 0; i < 8; i++) {
//         // We skip the 16 bytes of header to see user data
//         printf("%02x ", (unsigned char)chunk->raw_bytes[16 + i]);
//     }
//     printf("\n\n");
// }

// int main() {
//     printf("=== Understanding chunk layout ===\n\n");
    
//     void* p1 = malloc(20);
//     void* p2 = malloc(20);
    
//     // Get chunk pointers
//     void* chunk1 = (char*)p1 - 16;
//     void* chunk2 = (char*)p2 - 16;
    
//     printf("Chunk 1 starts at: %p\n", chunk1);
//     printf("User p1 at:        %p (chunk1 + 16)\n", p1);
//     printf("Chunk 2 starts at: %p\n", chunk2);
//     printf("User p2 at:        %p (chunk2 + 16)\n\n", p2);
    
//     printf("chunk2 - chunk1 = %ld bytes\n", (char*)chunk2 - (char*)chunk1);
//     printf("p2 - p1 = %ld bytes\n\n", (char*)p2 - (char*)p1);
    
//     // Where does p1's usable space end?
//     printf("P1's memory layout:\n");
//     printf("  [chunk1 + 0  to +7]:  prev_size field (8 bytes)\n");
//     printf("  [chunk1 + 8  to +15]: size_field (8 bytes)\n");
//     printf("  [chunk1 + 16 to +31]: USER DATA AREA (16 bytes) ← p1 points here\n");
//     printf("  [chunk1 + 32]:        START OF CHUNK2\n\n");
    
//     printf("If p1 points to chunk1+16, and chunk2 starts at chunk1+32,\n");
//     printf("then p1 has exactly (32-16) = 16 bytes of SAFE user space.\n\n");
    
//     // Test writing exactly 16 bytes
//     printf("=== Test: Writing exactly 16 bytes (safe) ===\n");
//     for(int i = 0; i < 16; i++) {
//         ((char*)p1)[i] = 'A' + i;
//     }
    
//     inspect_chunk(p1);
//     inspect_chunk(p2);
    
//     printf("p2's prev_size should be 0 (uncorrupted): %s\n\n",
//            ((union HeapChunk*)chunk2)->header.prev_size == 0 ? "✓ PASS" : "✗ CORRUPTED");
    
//     // Test writing 20 bytes (what we requested)
//     printf("=== Test: Writing 20 bytes (requested amount) ===\n");
//     void* p3 = malloc(20);
//     void* p4 = malloc(20);
//     void* chunk4 = (char*)p4 - 16;
    
//     for(int i = 0; i < 20; i++) {
//         ((char*)p3)[i] = '0' + (i % 10);
//     }
    
//     inspect_chunk(p3);
//     inspect_chunk(p4);
    
//     size_t p4_prev = ((union HeapChunk*)chunk4)->header.prev_size;
//     printf("p4's prev_size = 0x%lx (should be 0 if safe): %s\n\n",
//            p4_prev, p4_prev == 0 ? "✓ PASS" : "✗ CORRUPTED");
    
//     free(p1);
//     free(p2);
//     free(p3);
//     free(p4);
    
//     return 0;
// }

int main (int argc, char* argv[]) {
    void *p1 = malloc(73);
    void *p2 = malloc(50);

    sprintf(p1, "Hello this is p1!");
    sprintf(p2, "Hello this is p2!");

    printf("p1(%zu): %s\n", strlen(p1), (char*)p1);
    printf("p2(%zu): %s\n", strlen(p2), (char*)p2);

    printf("Address p1: %p\n", p1 - 8);
    printf("Header of p1: %d\n", *(char*)(p1 - 8));
    printf("Address p2: %p\n", p2);

    printf("Address of p2 relative to p1: %ld\n", p2 - p1);

    free(p1);
    free(p2);
}