#ifndef __KERNEL_ALLOC_H
#define __KERNEL_ALLOC_H

#include <stdbool.h>
#include <stdint.h>

// BLOCK_END:
// - When used in previ: this is root node
// - When used in nexti: this is last node
// - When used in rooti: there are no nodes
#define BLOCK_END  0xFF

// Size of a block (4kiB)
#define BLOCK_PO2  12
#define BLOCK_SIZE (1<<BLOCK_PO2)

typedef uint8_t block_t[BLOCK_SIZE];
typedef struct { uint8_t previ; uint8_t nexti; } BlockNode;

typedef struct {
  BlockNode* nodes;
  block_t*   blocks;
  uint8_t    cap;     // number of blocks
  uint8_t    rooti;   // root index
} BlockAllocator;

typedef struct {
  BlockAllocator* ba;
  uint8_t         ba_rooti; // owned block
  uint16_t        len;      // lower (unaligned) used bytes
  uint16_t        cap;      // upper (aligned) used bytes
} BlockBumpArena;

typedef struct {
  void* data;            // data of the size requested
  void* leftover;        // leftover data
  uint16_t leftoverSize;
} BBAReturn;

void     kernel_hello();
#define  BA_index(BA, BLOCK)   ( \
    ((uint8_t*) (BLOCK) - (uint8_t*) (BA)->blocks) \
    >> BLOCK_PO2)
void     BA_init     (BlockAllocator* a);
block_t* BA_alloc    (BlockAllocator* a, uint8_t* clientRooti);
void     BA_free     (BlockAllocator* a, uint8_t* clientRooti, block_t* b);
void     BA_freeAll  (BlockAllocator* a, uint8_t* clientRooti);

#define    BBA_new(BA) { .ba=BA, .ba_rooti=BLOCK_END, .len=0, .cap=0 };
BBAReturn  BBA_alloc           (BlockBumpArena* bba, uint16_t size);
BBAReturn  BBA_allocUnaligned  (BlockBumpArena* bba, uint16_t size);
#define    BBA_drop(BBA)       BA_freeAll((BBA).ba, &(BBA).ba_rooti)

// Convience macro to malloc new block allocator. Remember to free it!
#define BA_new(BA, NUM_BLOCKS)                                  \
    (BA).cap = NUM_BLOCKS;                                      \
    (BA).nodes = malloc((NUM_BLOCKS) << 1); /* NUM_BLOCKS*2 */  \
    (BA).blocks = malloc((NUM_BLOCKS) << BLOCK_PO2);            \
    BA_init(&(BA));

// Convienience macro to drop block allocator reserved using malloc.
#define BA_drop(BA)         \
    free((BA).nodes);         \
    free((BA).blocks);        \
    BA.cap = 0;             \
    BA.rooti = BLOCK_END;

#endif // __KERNEL_ALLOC_H
