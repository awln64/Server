#pragma once
#include <string>
#include "json.hpp"

using json = nlohmann::json;

struct TodoItem {
    int id;
    std::string description;
    std::string status;

    json to_json() const {
        return json{ {"id", id}, {"description", description}, {"status", status} };
    }
};