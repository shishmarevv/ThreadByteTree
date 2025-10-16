/*
 * @author: Viktor Shishmarev
 * @date: 14.10.2025
 * @description:
 */

#pragma once

#include "comparator.h"
#include <utility>
#include <vector>
#include <mutex>

namespace tbt{

    class Node {
        public:
            ByteVector Key;
            ByteVector Value;
            std::vector<Node*> Forward;

            Node(const ByteVector& key, const ByteVector& value, const std::size_t height): Key(key), Value(value), Forward(height, nullptr){}
    };

    class List {
        private:
            Node* head;
            std::size_t maxLevel;
            std::size_t currentLevel;
            float probability;
            std::mutex mux;

            void clear() const;
        public:
            List(std::size_t maxLevel, float probability);
            ~List();

            void Insert(const ByteVector& key, const ByteVector& value);
            // void Remove(const ByteVector& data);
            ByteVector Search(const ByteVector& key) const;

            // void ReCalibrate();

            // void ChangeProbability(float newProbability);
            // void ChangeMaxLevel(std::size_t newMaxLevel);
    };

    bool checkProbability(float probability);
}