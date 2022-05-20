# Kernel Alloc

This is a small (single header, single source) C library for kernel memory
managers for the [fngi] language, or anyone else who wants to use them.

Library Contents:

- `BlockAllocator`: an allocator for 4KiB blocks of memory with O(1) alloc/free
  performance. Utilizes 8bit indexes for extremely compact management of both
  allocated and free blocks. Can manage almost a full MiB of memory (manages
  up to `1MiB - 4KiB`).
- `BlockBumpArena` allocator: an arena allocator that efficiently "bump
  allocates" memory of any size less than a block (aligned or unaligned). It
  gets it's blocks of memory from a BlockAllocator. Dropping the arena will drop
  all the blocks it owns, but otherwise it has no mechanism to free memory so
  use for applications that don't alloc/free dynamically (but might free an
  entire arena).

The BlockAllocator is suitable for building many other low-level allocators on
top of (i.e. buddy, slab, etc). Because memory is allocated and freed in 4KiB
blocks, there can be no fragmentation if used correctly.

The BlockBumpArena is built on the BlockAllocator and is suitable (among other
usecases) for storing a "code heap" and "code metadata" (i.e.  a dictionary
using a BST) on top of. In [fngi] it is used to store module-level code and
metadata -- like function names, constants, types, etc -- which can be dropped
if/when a module is no longer needed.

## LICENSING

This work is part of the Civboot project and therefore primarily exists for
educational purposes. Attribution to the authors and project is appreciated but
not necessary.

Therefore this body of work is licensed using the [UNLICENSE](./UNLICENSE),
unless otherwise specified at the beginning of the source file.

If for any reason the UNLICENSE is not valid in your jurisdiction or project,
this work can be singly or dual licensed at your discression with the MIT
license below.

```text
Copyright 2022 Garrett Berg

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

[fngi]: https://github.com/civboot/fngi
