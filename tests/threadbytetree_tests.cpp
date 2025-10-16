/*
 * Tests for ThreadByteTree only: basic put/get/update and concurrent writers.
 */

#include <iostream>
#include <vector>
#include <thread>

#include "ThreadByteTree.h"

using namespace tbt;

static ByteVector key_of(int x) {
    // 4-byte big-endian representation to preserve numeric order under lexicographic compare
    ByteVector v(4);
    v[0] = static_cast<uint8_t>((x >> 24) & 0xFF);
    v[1] = static_cast<uint8_t>((x >> 16) & 0xFF);
    v[2] = static_cast<uint8_t>((x >> 8) & 0xFF);
    v[3] = static_cast<uint8_t>(x & 0xFF);
    return v;
}

static ByteVector val_of(int x) {
    // Single byte value mod 256
    return ByteVector{static_cast<uint8_t>(x & 0xFF)};
}

static bool test_threadbytetree_basic() {
    ThreadByteTree tbtree(16, 0.5f);
    tbtree.put(key_of(42), val_of(7));
    auto v = tbtree.get(key_of(42));
    if (!ByteVectorEqual(v, val_of(7))) return false;

    // Missing
    if (!tbtree.get(key_of(1000)).empty()) return false;

    // Update
    tbtree.put(key_of(42), val_of(8));
    if (!ByteVectorEqual(tbtree.get(key_of(42)), val_of(8))) return false;

    return true;
}

static bool test_threadbytetree_concurrency() {
    ThreadByteTree tbtree(20, 0.5f);
    const int writers = 3;
    const int per_writer = 1500;

    std::vector<std::thread> threads;
    for (int w = 0; w < writers; ++w) {
        threads.emplace_back([w, per_writer, &tbtree]() {
            int start = w * per_writer;
            int end = start + per_writer;
            for (int i = start; i < end; ++i) {
                tbtree.put(key_of(i), val_of(i));
            }
        });
    }

    for (auto &t : threads) t.join();

    for (int i = 0; i < writers * per_writer; ++i) {
        auto got = tbtree.get(key_of(i));
        if (got.empty() || !ByteVectorEqual(got, val_of(i))) {
            std::cerr << "threadbytetree_concurrency wrong value at " << i << "\n";
            return false;
        }
    }
    return true;
}

int main() {
    int failed = 0;
    auto run = [&](const char* name, bool (*fn)()) {
        bool ok = fn();
        std::cout << (ok ? "OK: " : "FAIL: ") << name << "\n";
        if (!ok) ++failed;
    };

    run("threadbytetree_basic", &test_threadbytetree_basic);
    run("threadbytetree_concurrency", &test_threadbytetree_concurrency);

    if (failed == 0) {
        std::cout << "All ThreadByteTree tests passed" << std::endl;
        return 0;
    }
    std::cerr << failed << " ThreadByteTree test(s) failed" << std::endl;
    return 1;
}
