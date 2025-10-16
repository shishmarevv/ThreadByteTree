/*
 * @author: Viktor Shishmarev
 * @date: 16.10.2025
 * @description: Small wrapper around a concurrent SkipList to provide a simple
 * key-value API with byte-vector keys and values. Exposes thread-safe put/get.
 */


#pragma once

#include "skiplist.h"

namespace tbt {

    class ThreadByteTree {
    private:
        List skipList;

    public:
        /*
         * Construct a ThreadByteTree backed by a SkipList.
         * Parameters:
         *   - maxLevel: number of levels in the internal skip list (>=1), indexed 0..maxLevel-1.
         *   - probability: node promotion probability used by the skip list; must be in (0,1).
         * Returns:
         *   - N/A
         * Throws:
         *   - std::invalid_argument if probability is not strictly between 0 and 1 (propagated from List).
         * Effects:
         *   - Initializes the internal skip list with the specified parameters.
         */
        ThreadByteTree(std::size_t maxLevel, float probability);

        /*
         * Insert or update a value by key (synchronous, thread-safe).
         * Parameters:
         *   - key: byte-vector key.
         *   - value: byte-vector value to associate with key.
         * Returns:
         *   - N/A
         * Effects:
         *   - If the key exists, its value is replaced; otherwise a new entry is created.
         * Thread-safety:
         *   - Safe for concurrent calls; internally serialized for writers.
         */
        void put(const ByteVector& key, const ByteVector& value);

        /*
         * Retrieve a value by key (synchronous, thread-safe).
         * Parameters:
         *   - key: byte-vector key to search for.
         * Returns:
         *   - Associated value if found; otherwise an empty ByteVector.
         * Thread-safety:
         *   - Safe for concurrent calls; multiple readers proceed concurrently.
         */
        ByteVector get(const ByteVector& key) const;
    };

}
