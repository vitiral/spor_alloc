#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "kernel_alloc.h"

void testNew() {
  printf("## testNew\n");
  BlockAllocator ba;
  BA_new(ba, 4); // root -> a -> b -> c -> d
  assert(4 == ba.cap);
  // Check start node: root <-> a
  assert(BLOCK_END == ba.nodes[0].previ);
  assert(1         == ba.nodes[0].nexti);

  // Check end node: c <- d -> END
  assert(2         == ba.nodes[3].previ);
  assert(BLOCK_END == ba.nodes[3].nexti);
  BA_drop(ba);
}

void testAllocFree() {
  printf("## testAllocFree\n");
  uint8_t crooti = BLOCK_END; // clientRoot
  BlockAllocator ba;
  BA_new(ba, 4); // baRoot -> a -> b -> c -> d

  block_t* a = BA_alloc(&ba, &crooti);
  assert(a == ba.blocks); // first block

  // clientroot -> a
  assert(0         == crooti);
  assert(BLOCK_END == ba.nodes[0].previ);
  assert(BLOCK_END == ba.nodes[0].nexti);

  // baRoot -> b -> c
  assert(1         == ba.rooti);
  assert(BLOCK_END == ba.nodes[1].previ);
  assert(2         == ba.nodes[1].nexti);

  BA_free(&ba, &crooti, a);
  assert(BLOCK_END == crooti);
  assert(BLOCK_END == ba.nodes[0].previ);
  assert(1         == ba.nodes[0].nexti);
  assert(0         == ba.nodes[1].previ);

  BA_drop(ba);
}

void testAlloc2FreeFirst() {
  printf("## testAlloc2FreeFirst\n");
  uint8_t crooti = BLOCK_END; // clientRoot
  BlockAllocator ba;
  BA_new(ba, 4); // baRoot -> a -> b -> c -> d

  block_t* a = BA_alloc(&ba, &crooti);
  block_t* b = BA_alloc(&ba, &crooti); // clientRoot -> b -> a;  baRoot -> c -> d
  assert(a == ba.blocks);     // first block
  assert(b == &ba.blocks[1]); // second block

  BA_free(&ba, &crooti, a); // clientroot -> b -> END;  baRoot -> a -> c -> d

  // clientroot -> b -> END
  assert(1         == crooti);
  assert(BLOCK_END == ba.nodes[1].previ);
  assert(BLOCK_END == ba.nodes[1].nexti);

  // baRoot -> a -> c ...
  assert(0         == ba.rooti);
  assert(BLOCK_END == ba.nodes[0].previ);
  assert(2         == ba.nodes[0].nexti);

  BA_drop(ba);
}

void testBBA() {
  printf("## testBBA\n");
  uint8_t crooti = BLOCK_END; // clientRoot
  BlockAllocator ba;
  BA_new(ba, 4);
  BlockBumpArena bba = BBA_new(&ba);

  BBAReturn ret = BBA_alloc(&bba, 12);
  assert(0                               == ret.leftoverSize);
  assert(&ba.blocks[0] + BLOCK_SIZE - 12 == ret.data);

  ret = BBA_alloc(&bba, BLOCK_SIZE);
  assert(BLOCK_SIZE - 12                 == ret.leftoverSize);
  assert(&ba.blocks[0]                   == ret.leftover);
  assert(&ba.blocks[1]                   == ret.data);

  ret = BBA_allocUnaligned(&bba, 13);
  assert(0                               == ret.leftoverSize);
  assert(&ba.blocks[2]                   == ret.data);

  ret = BBA_allocUnaligned(&bba, 25);
  assert(0                               == ret.leftoverSize);
  assert(&ba.blocks[2] + 13              == ret.data);

  ret = BBA_allocUnaligned(&bba, BLOCK_SIZE - 20);
  assert(BLOCK_SIZE - 13 - 25            == ret.leftoverSize);
  assert(&ba.blocks[3]                   == ret.data);

  ret = BBA_alloc(&bba, BLOCK_SIZE);
  assert(20                              == ret.leftoverSize);
  assert(NULL                            == ret.data);

  BBA_drop(bba);
  BA_drop(ba);
}

int main() {
  printf("Starting Tests\n");
  testNew();
  testAllocFree();
  testAlloc2FreeFirst();
  testBBA();
}
