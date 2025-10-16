/*
 * @author: Viktor Shishmarev
 * @date: 16.10.2025
 * @description:
 */

#include "skiplist.h"

#include <stdexcept>

namespace tbt {
    bool checkProbability(const float probability) {
        return probability > 0.0f && probability < 1.0f;
    }

    bool toss(const float probability) {
        return (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) < probability;
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

        head = new Node(ByteVector(), maxLevel);
        this->maxLevel = maxLevel;
        this->currentLevel = 0;
        this->probability = probability;
        this->size = 0;
    }

    List::~List() {
        std::lock_guard lock(mux);

        clear();
        delete head;
    }

    Node *List::Search(const ByteVector &data) const {
        Node *current = head;
        for (std::size_t i = currentLevel + 1; i-- > 0;) {
            while (current -> Forward[i] != nullptr && ByteVectorLess(current -> Data, data)) {
                current = current -> Forward[i];
            }
        }
        if (current -> Forward[0] != nullptr && ByteVectorEqual(current -> Forward[0] -> Data, data)) {
            return current -> Forward[0];
        }
        return nullptr;
    }

    void List::Insert(const ByteVector &data) {
        std::size_t newLevel = 0;

        while (newLevel < maxLevel && toss(probability)) {
            newLevel++;
        }

        if (currentLevel < newLevel) {
            std::lock_guard lock(mux);
            head -> Forward.resize(newLevel + 1, nullptr);
            currentLevel = newLevel;
        }

        Node* current = head;
        std::vector<Node*> update(currentLevel + 1, nullptr);

        for (std::size_t i = currentLevel + 1; i-- > 0;) {
            while (current -> Forward[i] != nullptr && ByteVectorLess(current -> Data, data)) {
                current = current -> Forward[i];
            }
            update[i] = current;
        }

        current = current->Forward[0];

        if (current == nullptr || !ByteVectorEqual(current->Data, data)) {
            std::lock_guard lock(mux);
            Node *newNode = new Node(data, newLevel);

            for (std::size_t i = 0; i <= newLevel; i++) {
                newNode -> Forward[i] = update[i] -> Forward[i];
                update[i] -> Forward[i] = newNode;
            }
            size++;
        } else {
            // Element already exists, do nothing
        }
    }

    void List::Remove(const ByteVector &data) {
        Node *current = head;
        std::vector<Node*> update(currentLevel + 1, nullptr);

        for (std::size_t i = currentLevel + 1; i-- > 0;) {
            while (current -> Forward[i] != nullptr && ByteVectorLess(current -> Data, data)) {
                current = current -> Forward[i];
            }
            update[i] = current;
        }

        current = current->Forward[0];

        if (current != nullptr && ByteVectorEqual(current -> Data, data)) {
            std::lock_guard lock(mux);
            for (std::size_t i = 0; i <= currentLevel; i++) {
                if (update[i] -> Forward[i] != current) {
                    break;
                }
                update[i] -> Forward[i] = current -> Forward[i];
            }
            delete current;

            while (currentLevel > 0 && head -> Forward[currentLevel] == nullptr) {
                currentLevel--;
            }

            size--;
        } else {
            // Element not found, do nothing
        }
    }

    void List::ReCalibrate() {
        std::lock_guard lock(mux);

        std::vector <ByteVector> elements;
        Node *current = head;
        elements.reserve(size);

        while (current -> Forward[0] != nullptr) {
            current = current -> Forward[0];
            elements.push_back(current -> Data);
        }

        clear();
        head -> Forward.assign(maxLevel, nullptr);
        currentLevel = 0;
        size = 0;

        for (const auto &element : elements) {
            Insert(element);
        }
    }

    void List::ChangeProbability(const float newProbability) {
        if (!checkProbability(probability)) {
            throw std::invalid_argument("probability must be between 0 and 1");
        }

        std::lock_guard lock(mux);

        probability = newProbability;
        ReCalibrate();
    }

    void List::ChangeMaxLevel(std::size_t newMaxLevel) {
        std::lock_guard lock(mux);

        maxLevel = newMaxLevel;
        ReCalibrate();
    }


}
