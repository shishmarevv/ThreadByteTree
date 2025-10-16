/*
 * @author: Viktor Shishmarev
 * @date: 16.10.2025
 * @description:
 */

#include "ThreadByteTree.h"
#include <thread>
#include <future>

namespace tbt {

    ThreadByteTree::ThreadByteTree(std::size_t maxLevel, float probability)
        : skipList(maxLevel, probability) {}

    void ThreadByteTree::put(const ByteVector& key, const ByteVector& value) {
        std::thread([this, key, value] {
            skipList.Insert(key, value);
        }).detach();
    }

    ByteVector ThreadByteTree::get(const ByteVector& key) const {
        auto future = std::async(std::launch::async, [this, key] {
            return skipList.Search(key);
        });

        return future.get();
    }

}
