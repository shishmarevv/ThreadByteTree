# ThreadByteTree

ThreadByteTree is a small C++ library that implements a thread-safe, sorted in-memory key-value store built on top of a custom SkipList. Keys and values are byte arrays (std::vector<uint8_t>), suitable for arbitrary binary data. Concurrency is provided via a read-write lock (std::shared_mutex): multiple readers can search concurrently, while writers obtain exclusive access only for the duration of modifications.

## Project logic
- Core data structure: SkipList (`tbt::List`) providing expected average O(log n) for search and insert.
- Skip list nodes store forward pointers across multiple levels (0..L) enabling fast jumps.
- Thread-safety via `std::shared_mutex`:
  - `Search` runs under a shared lock (many concurrent readers).
  - `Insert` runs under a unique lock (exclusive writer during mutation only).
- Public interface: `tbt::ThreadByteTree` exposing `put` and `get` as a thin, synchronous wrapper around the skip list.

## Original assignment (verbatim)
Please implement a thread-safe version of a sorted in-memory tree using a data structure of your preference. Do not use the existing implementation of data structures. Implement your own instead. Solution that delegates execution to implementing data structures from libraries will be rejected.

Implement only get and put methods and nothing more.

Keys and values of this tree are byte[] arrays.

## Why SkipList and a read-write lock
- SkipList:
  - Simpler to implement and maintain than self-balancing trees, while offering expected O(log n) operations.
  - Naturally amenable to concurrent access because of simple pointer structure.
- Read-Write Lock (`std::shared_mutex`):
  - Allows many reads to proceed in parallel, which is critical for read-heavy scenarios.
  - Writers hold the exclusive lock only briefly during structural changes, minimizing contention.

## Repository layout
- `include/comparator.h`, `src/comparator.cpp` — utilities for comparing ByteVector (lexicographic order and equality).
- `include/skiplist.h`, `src/skiplist.cpp` — thread-safe SkipList implementation (`Node` and `List`).
- `ThreadByteTree.h`, `src/ThreadByteTree.cpp` — interface (`ThreadByteTree`) with `put` and `get`.
- `tests/comparator.cpp` — comparator tests.
- `tests/*_tests.cpp` — split tests for SkipList and ThreadByteTree, including multithreaded scenarios.

## API summary
- `tbt::List`:
  - `List(std::size_t maxLevel, float probability)` — create a skip list with a given number of levels and a promotion probability in (0,1).
  - `void Insert(const ByteVector& key, const ByteVector& value)` — insert or update a key-value pair.
  - `ByteVector Search(const ByteVector& key) const` — find a value by key; returns an empty `ByteVector` if not found.
- `tbt::ThreadByteTree`:
  - `ThreadByteTree(std::size_t maxLevel, float probability)` — construct the store.
  - `void put(const ByteVector& key, const ByteVector& value)` — insert/update (synchronous).
  - `ByteVector get(const ByteVector& key) const` — search (synchronous).

Note: `maxLevel` is the number of levels (count), indexed 0..maxLevel-1. Each inserted node is assigned a random height according to `probability`.

## Integration
Option A. Add this project as a subdirectory in your CMake build:

1) Clone this repository into your project, e.g., `external/ThreadByteTree`.
2) In your `CMakeLists.txt`:

```
add_subdirectory(external/ThreadByteTree)

add_executable(my_app main.cpp)

target_link_libraries(my_app PRIVATE threadbytetree)
```

3) In your code:
```
#include "ThreadByteTree.h"
using namespace tbt;
```

Option B. Build `threadbytetree` as a static library and link it in your project. Public include directories are already configured in this repository's `CMakeLists.txt`.

## Building and tests
The project uses CMake. In CLion a build profile and targets are provided:
- Library: `threadbytetree`.
- Tests: `threadbytetree_tests_skiplist` (SkipList) and `threadbytetree_tests_threadbytetree` (ThreadByteTree), `threadbytetree_tests_comparator` (comparator).

Example: build and run the test targets from CLion or via CTest if enabled.

## Concurrency guarantees
- `std::shared_mutex` is used:
  - `Search` holds `std::shared_lock` allowing concurrent reads.
  - `Insert` holds `std::unique_lock` ensuring correct pointer updates and `currentLevel` maintenance.
- All structural modifications (node insertion, value updates, changing top level) are protected by the exclusive lock.
- Searches run entirely under a shared lock, preventing races with writers.

## Why ThreadByteTree does not spawn threads internally
`ThreadByteTree::put` and `get` are synchronous by design. Spawning a new thread for each call would:
- Change the observable semantics (e.g., a `put` returning before the value is visible to subsequent `get`).
- Add significant overhead and reduce performance under load.
- Provide no benefit over external concurrency: the data structure is already safe for many threads calling `put`/`get` concurrently.
If asynchronous behavior is desired, a separate API (e.g., `putAsync`/`getAsync` returning `std::future`) could be added without changing the semantics of the basic methods.

## Key/value notes
- Keys and values are arbitrary `std::vector<uint8_t>`.
- Key ordering is lexicographic on unsigned bytes (see `ByteVectorLess`). For numeric keys, prefer fixed-width big-endian encoding to preserve natural numeric order.
