#include "ThreadByteTree.h"

namespace tbt {

    ThreadByteTree::ThreadByteTree(std::size_t maxLevel, float probability)
        : skipList(maxLevel, probability) {}

    void ThreadByteTree::put(const ByteVector& key, const ByteVector& value) {
        skipList.Insert(key, value);
    }

    ByteVector ThreadByteTree::get(const ByteVector& key) const {
        return skipList.Search(key);
    }

}
