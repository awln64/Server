#pragma once
#include "Todo.hpp"
#include <vector>
#include <mutex>
#include <atomic>
#include <optional>

class TodoManager {
public:
    TodoManager() : next_id_(1) {}

    std::vector<TodoItem> list() {
        std::lock_guard<std::mutex> lk(mutex_);
        return items_;
    }

    TodoItem add(const std::string& description) {
        TodoItem item;
        item.id = next_id_.fetch_add(1);
        item.description = description;
        item.status = "Pending";
        {
            std::lock_guard<std::mutex> lk(mutex_);
            items_.push_back(item);
        }
        return item;
    }

    std::optional<TodoItem> toggle(int id) {
        std::optional<TodoItem> result;
        {
            std::lock_guard<std::mutex> lk(mutex_);
            for (auto& it : items_) {
                if (it.id == id) {
                    it.status = (it.status == "Pending") ? "Completed" : "Pending";
                    result = it;
                    break;
                }
            }
        }
        return result;
    }

private:
    std::vector<TodoItem> items_;
    std::mutex mutex_;
    std::atomic<int> next_id_;
};