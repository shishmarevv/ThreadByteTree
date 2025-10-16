/*
 * Tests for SkipList (List) only: basic insert/search/update and concurrent usage.
 */

#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <random>
#include <cassert>

#include "include/skiplist.h"

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

static bool test_skiplist_basic() {
    List list(16, 0.5f);
    // Insert
    list.Insert(key_of(1), val_of(10));
    list.Insert(key_of(2), val_of(20));
    list.Insert(key_of(3), val_of(30));

    // Search existing
    if (!ByteVectorEqual(list.Search(key_of(1)), val_of(10))) return false;
    if (!ByteVectorEqual(list.Search(key_of(2)), val_of(20))) return false;
    if (!ByteVectorEqual(list.Search(key_of(3)), val_of(30))) return false;
    // Search missing
    if (!list.Search(key_of(99)).empty()) return false;

    // Update existing key
    list.Insert(key_of(2), val_of(200));
    if (!ByteVectorEqual(list.Search(key_of(2)), val_of(200))) return false;

    return true;
}

static bool test_skiplist_concurrency() {
    List list(18, 0.5f);

    const int writers = 4;
    const int per_writer = 2000;
    std::vector<std::thread> threads;

    // Concurrent writers on disjoint key ranges
    for (int w = 0; w < writers; ++w) {
        threads.emplace_back([w, per_writer, &list]() {
            int start = w * per_writer;
            int end = start + per_writer;
            for (int i = start; i < end; ++i) {
                list.Insert(key_of(i), val_of(i));
            }
        });
    }

    // Concurrent readers while writing
    std::atomic<bool> stop{false};
    std::thread reader([&]() {
        std::mt19937 rng(12345);
        std::uniform_int_distribution<int> dist(0, writers * per_writer - 1);
        while (!stop.load(std::memory_order_relaxed)) {
            int k = dist(rng);
            (void)list.Search(key_of(k));
        }
    });

    for (auto &t : threads) t.join();
    stop.store(true);
    reader.join();

    // Verify all keys exist
    for (int i = 0; i < writers * per_writer; ++i) {
        auto got = list.Search(key_of(i));
        if (got.empty() || !ByteVectorEqual(got, val_of(i))) {
            std::cerr << "skiplist_concurrency missing or wrong value at " << i << "\n";
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

    run("skiplist_basic", &test_skiplist_basic);
    run("skiplist_concurrency", &test_skiplist_concurrency);

    if (failed == 0) {
        std::cout << "All SkipList tests passed" << std::endl;
        return 0;
    }
    std::cerr << failed << " SkipList test(s) failed" << std::endl;
    return 1;
}
