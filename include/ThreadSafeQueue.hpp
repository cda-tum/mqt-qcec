/*
* This file is part of MQT QCEC library which is released under the MIT license.
* See file README.md or go to https://www.cda.cit.tum.de/research/quantum_verification/ for more information.
*/

#pragma once

#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>

template<typename T>
class ThreadSafeQueue {
private:
    struct Node {
        std::shared_ptr<T>    data;
        std::unique_ptr<Node> next;
    };
    std::mutex              headMutex;
    std::unique_ptr<Node>   head;
    std::mutex              tailMutex;
    Node*                   tail;
    std::condition_variable dataCond;

    auto getTail() {
        std::lock_guard tailLock(tailMutex);
        return tail;
    }

    auto popHead() {
        auto oldHead = std::move(head);
        head         = std::move(oldHead->next);
        return oldHead;
    }

    auto waitForData() {
        std::unique_lock headLock(headMutex);
        dataCond.wait(headLock, [&] { return head.get() != getTail(); });
        return headLock;
    }

    template<typename Clock, typename Dur>
    auto waitForDataUntil(const std::chrono::time_point<Clock, Dur>& timepoint) {
        std::unique_lock headLock(headMutex);
        dataCond.wait_until(headLock, timepoint, [&] { return head.get() != getTail(); });
        return headLock;
    }

    auto waitPopHead() {
        auto headLock(waitForData());
        return popHead();
    }

    template<typename Clock, typename Dur>
    auto waitPopHeadUntil(const std::chrono::time_point<Clock, Dur>& timepoint) {
        auto headLock(waitForDataUntil(timepoint));
        if (head.get() == getTail()) {
            return std::unique_ptr<Node>();
        }
        return popHead();
    }

public:
    ThreadSafeQueue():
        head(std::make_unique<Node>()), tail(head.get()) {}

    ThreadSafeQueue(const ThreadSafeQueue& other) = delete;

    ThreadSafeQueue& operator=(const ThreadSafeQueue& other) = delete;

    std::shared_ptr<T> waitAndPop() {
        auto const oldHead = waitPopHead();
        return oldHead->data;
    }

    template<typename Clock, typename Dur>
    std::shared_ptr<T> waitAndPopUntil(const std::chrono::time_point<Clock, Dur>& timepoint) {
        auto const oldHead = waitPopHeadUntil(timepoint);
        return oldHead ? oldHead->data : std::shared_ptr<T>();
    }

    void push(T value) {
        auto data(std::make_shared<T>(std::move(value)));
        auto p = std::make_unique<Node>();
        {
            std::lock_guard tailLock(tailMutex);
            tail->data          = data;
            Node* const newTail = p.get();
            tail->next          = std::move(p);
            tail                = newTail;
        }
        dataCond.notify_one();
    }
    bool empty() {
        std::lock_guard headLock(headMutex);
        return (head.get() == getTail());
    }
};
