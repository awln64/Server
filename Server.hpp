#pragma once
#include "IRequestHandler.hpp"
#include "Session.hpp"
#include "TodoManager.hpp"
#include <asio.hpp>
#include <set>
#include <memory>
#include <mutex>

class Server : public IRequestHandler {
public:
    Server(asio::io_context& io_context, short port);

    void handle_request(const nlohmann::json& req, std::shared_ptr<Session> session) override;
    void leave(std::shared_ptr<Session> session) override;
    void broadcast(const std::string& msg) override;

private:
    void do_accept();
    void join(std::shared_ptr<Session> session);

    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::set<std::shared_ptr<Session>> sessions_;
    std::mutex sessions_mutex_;
    TodoManager todo_manager_;
};
