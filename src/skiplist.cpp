#include "skiplist.h"

#include <stdexcept>
#include <random>
#include <shared_mutex>
#include <mutex>

namespace tbt {
    bool checkProbability(const float probability) {
        return probability > 0.0f && probability < 1.0f;
    }

    bool toss(const float probability) {
        thread_local std::mt19937 generator(std::random_device{}());
        std::uniform_real_distribution distribution(0.0f, 1.0f);
        return distribution(generator) < probability;
    }

    void List::clear() const {
        Node* current = head -> Forward[0];
        while (current != nullptr) {
            Node *next = current->Forward[0];
            delete current;
            current = next;
        }
    }

    List::List(const std::size_t maxLevel, const float probability) {
        if (!checkProbability(probability)) {
            throw std::invalid_argument("probability must be between 0 and 1");
        }

        head = new Node(ByteVector(), ByteVector(), maxLevel);
        this->maxLevel = maxLevel;
        this->currentLevel = 0;
        this->probability = probability;
    }

    List::~List() {
        std::unique_lock<std::shared_mutex> lock(mux);
        clear();
        delete head;
    }

    ByteVector List::Search(const ByteVector &key) const {
        std::shared_lock<std::shared_mutex> lock(mux);
        Node *current = head;
        for (std::size_t i = currentLevel + 1; i-- > 0;) {
            while (current->Forward[i] != nullptr && ByteVectorLess(current->Forward[i]->Key, key)) {
                current = current->Forward[i];
            }
        }
        Node* next = current->Forward[0];
        if (next != nullptr && ByteVectorEqual(next->Key, key)) {
            return next->Value;
        }
        return {};
    }

    void List::Insert(const ByteVector& key, const ByteVector& value) {
        std::unique_lock<std::shared_mutex> lock(mux);

        std::size_t newLevel = 0;
        while ((newLevel + 1) < maxLevel && toss(probability)) {
            newLevel++;
        }

        if (currentLevel < newLevel) {
            currentLevel = newLevel;
        }

        Node* current = head;
        std::vector<Node*> update(currentLevel + 1, nullptr);

        for (std::size_t i = currentLevel + 1; i-- > 0;) {
            while (current->Forward[i] != nullptr && ByteVectorLess(current->Forward[i]->Key, key)) {
                current = current->Forward[i];
            }
            update[i] = current;
        }

        Node* next = current->Forward[0];

        if (next == nullptr || !ByteVectorEqual(next->Key, key)) {
            Node *newNode = new Node(key, value, newLevel + 1);

            for (std::size_t i = 0; i <= newLevel; i++) {
                newNode->Forward[i] = update[i]->Forward[i];
                update[i]->Forward[i] = newNode;
            }
        } else {
            next->Value = value; // Update existing value
        }
    }

    // Unnecessary functionality
    //
    // void List::Remove(const ByteVector &data) {
    //     Node *current = head;
    //     std::vector<Node*> update(currentLevel + 1, nullptr);
    //
    //     for (std::size_t i = currentLevel + 1; i-- > 0;) {
    //         while (current -> Forward[i] != nullptr && ByteVectorLess(current -> Data, data)) {
    //             current = current -> Forward[i];
    //         }
    //         update[i] = current;
    //     }
    //
    //     current = current->Forward[0];
    //
    //     if (current != nullptr && ByteVectorEqual(current -> Data, data)) {
    //         std::lock_guard lock(mux);
    //         for (std::size_t i = 0; i <= currentLevel; i++) {
    //             if (update[i] -> Forward[i] != current) {
    //                 break;
    //             }
    //             update[i] -> Forward[i] = current -> Forward[i];
    //         }
    //         delete current;
    //
    //         while (currentLevel > 0 && head -> Forward[currentLevel] == nullptr) {
    //             currentLevel--;
    //         }
    //
    //         size--;
    //     } else {
    //         // Element not found, do nothing
    //     }
    // }
    //
    // void List::ReCalibrate() {
    //     std::lock_guard lock(mux);
    //
    //     std::vector <ByteVector> elements;
    //     Node *current = head;
    //     elements.reserve(size);
    //
    //     while (current -> Forward[0] != nullptr) {
    //         current = current -> Forward[0];
    //         elements.push_back(current -> Data);
    //     }
    //
    //     clear();
    //     head -> Forward.assign(maxLevel, nullptr);
    //     currentLevel = 0;
    //     size = 0;
    //
    //     for (const auto &element : elements) {
    //         Insert(element);
    //     }
    // }
    //
    // void List::ChangeProbability(const float newProbability) {
    //     if (!checkProbability(probability)) {
    //         throw std::invalid_argument("probability must be between 0 and 1");
    //     }
    //
    //     std::lock_guard lock(mux);
    //
    //     probability = newProbability;
    //     ReCalibrate();
    // }
    //
    // void List::ChangeMaxLevel(std::size_t newMaxLevel) {
    //     std::lock_guard lock(mux);
    //
    //     maxLevel = newMaxLevel;
    //     ReCalibrate();
    // }
}
