/*
 * @author: Viktor Shishmarev
 * @date: 14.10.2025
 * @description: This header declares a minimal skip list (List) for byte-vector keys and values,
 * and its building block Node. The structure is optimized for concurrent reads via
 * a read-write lock and supports put/get (insert/update and search).
 */

#pragma once

#include "comparator.h"
#include <utility>
#include <vector>
#include <shared_mutex>

namespace tbt{

    class Node {
        public:
            ByteVector Key;
            ByteVector Value;
            std::vector<Node*> Forward;

            /*
             * Construct a node that stores a key/value pair and forward pointers for skip list levels.
             * Parameters:
             *   - key: byte-vector key for this node.
             *   - value: byte-vector value associated with the key.
             *   - heightLevels: number of forward pointers (levels) for this node (>=1 for level 0).
             * Effects:
             *   - Initializes Forward with heightLevels null pointers.
             */
            Node(const ByteVector& key, const ByteVector& value, std::size_t heightLevels)
                : Key(key), Value(value), Forward(heightLevels, nullptr) {}
    };

    class List {
        private:
            Node* head;
            std::size_t maxLevel;
            std::size_t currentLevel;
            float probability;
            mutable std::shared_mutex mux;

            void clear() const;
        public:
            /*
             * Construct a skip list with a specified number of levels and promotion probability.
             * Parameters:
             *   - maxLevel: total number of levels available (>=1), indexed 0..maxLevel-1.
             *   - probability: node-promotion probability used for random height generation; must be in (0,1).
             * Returns:
             *   - N/A
             * Throws:
             *   - std::invalid_argument if probability is not strictly between 0 and 1.
             * Effects:
             *   - Allocates a sentinel head node with maxLevel forward pointers and initializes internal state.
             */
            List(std::size_t maxLevel, float probability);

            /*
             * Destroy the list and free all nodes.
             * Thread-safety:
             *   - Acquires an exclusive lock internally; should be invoked when no other operations are running.
             */
            ~List();

            /*
             * Insert or update a key with the given value.
             * Parameters:
             *   - key: byte-vector key (ordered lexicographically using unsigned byte semantics).
             *   - value: byte-vector value to associate with the key.
             * Returns:
             *   - N/A
             * Effects:
             *   - If the key exists, its value is replaced; otherwise a new node is inserted with a random height.
             * Thread-safety:
             *   - Acquires a unique (exclusive) lock; concurrent writers are serialized.
             * Complexity:
             *   - Expected O(log n) time.
             */
            void Insert(const ByteVector& key, const ByteVector& value);

            /*
             * Look up a key and return its associated value.
             * Parameters:
             *   - key: byte-vector key to search for.
             * Returns:
             *   - The associated value if found; otherwise an empty ByteVector.
             * Thread-safety:
             *   - Acquires a shared lock allowing multiple concurrent readers.
             * Complexity:
             *   - Expected O(log n) time.
             */
            ByteVector Search(const ByteVector& key) const;
    };

    /*
     * Validate that a probability value lies strictly between 0 and 1.
     * Parameters:
     *   - probability: value to validate.
     * Returns:
     *   - true if 0 < probability < 1; false otherwise.
     * Edge cases:
     *   - NaN input will return false due to comparison semantics.
     */
    bool checkProbability(float probability);

    /*
     * Toss a biased coin that returns true with the given probability.
     * Parameters:
     *   - probability: probability of success (true), expected in (0,1).
     * Returns:
     *   - true with the specified probability; false otherwise.
     * Notes:
     *   - Uses a thread-local PRNG; safe for concurrent calls from multiple threads.
     */
    bool toss(float probability);
}