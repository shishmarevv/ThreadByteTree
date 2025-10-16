/*
 * @author: Viktor Shishmarev
 * @date: 16.10.2025
 * @description:
 */

#pragma once

#pragma once

#include "skiplist.h"

namespace tbt {

    class ThreadByteTree {
    private:
        List skipList;

    public:
        ThreadByteTree(std::size_t maxLevel, float probability);

        void put(const ByteVector& key, const ByteVector& value);
        ByteVector get(const ByteVector& key) const;
    };

}
