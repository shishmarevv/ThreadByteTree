/*
 * @author: Viktor Shishmarev
 * @date: 14.10.2025
 * @description:
 */

#pragma once

#include <comparator.h>
#include <utility>
#include <vector>
#include <mutex>

namespace tbt{

    class Node {
        public:
            ByteVector Data;
            std::vector<Node*> Forward;

            Node(const ByteVector& data, const std::size_t height): Data(data), Forward(height, nullptr){}
    };

    class List {
        private:
            Node* head;
            std::size_t maxLevel;
            std::size_t currentLevel;
            std::size_t size;
            float probability;
            std::mutex mux;

            void clear() const;
        public:
            List(std::size_t maxLevel, float probability);
            ~List();

            void Insert(const ByteVector& data);
            void Remove(const ByteVector& data);
            Node* Search(const ByteVector& data) const;

            void ReCalibrate();

            void ChangeProbability(float newProbability);
            void ChangeMaxLevel(std::size_t newMaxLevel);
    };

    bool checkProbability(float probability);
}