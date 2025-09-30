#pragma once
#include <memory>
#include "json.hpp"

class Session;

class IRequestHandler {
public:
    virtual void handle_request(const nlohmann::json& req, std::shared_ptr<Session> session) = 0;
    virtual void leave(std::shared_ptr<Session> session) = 0;
    virtual void broadcast(const std::string& msg) = 0;
    virtual ~IRequestHandler() = default;
};
