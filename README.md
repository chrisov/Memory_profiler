Allocated Chunk:
┌─────────────────────┐
│  prev_size (8 bytes)│ (only if previous chunk is FREE)
├─────────────────────┤
│  size | flags (8)   │ ← mchunk_size (ALWAYS PRESENT)
├─────────────────────┤
│                     │
│     USER DATA       │ ← malloc() returns pointer here
│     (variable)      │
│                     │
├─────────────────────┤
│  size|flags (8)     │ ← next chunk's prev_size field
└─────────────────────┘

Free Chunk:
┌─────────────────────┐
│  prev_size (8)      │ 
├─────────────────────┤
│  size | flags (8)   │ 
├─────────────────────┤
│  fd (forward ptr)   │ ← pointer to next free chunk in bin
│  (8 bytes)          │
├─────────────────────┤
│  bk (back ptr)      │ ← pointer to prev free chunk in bin
│  (8 bytes)          │
├─────────────────────┤
│                     │
│   FREE SPACE        │
│                     │
├─────────────────────┤
│  size|flags (8)     │ ← next chunk's prev_size
└─────────────────────┘