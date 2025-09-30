#include "Server.hpp"
#include "Session.hpp"
#include <iostream>

Server::Server(asio::io_context& io_context, short port) : io_context_(io_context),
acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {

    auto ep = acceptor_.local_endpoint();
    std::cout << "Server listening on " << ep.address().to_string() << ":" << ep.port() << std::endl;

    do_accept();
}

void Server::join(std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lk(sessions_mutex_);
    sessions_.insert(session);
}

void Server::leave(std::shared_ptr<Session> session) {
    std::lock_guard<std::mutex> lk(sessions_mutex_);
    sessions_.erase(session);
}

void Server::broadcast(const std::string& msg) {
    std::vector<std::shared_ptr<Session>> sessions_copy;
    {
        std::lock_guard<std::mutex> lk(sessions_mutex_);
        sessions_copy.reserve(sessions_.size());
        for (auto& s : sessions_) sessions_copy.push_back(s);
    }
    for (auto& s : sessions_copy) {
        if (s) s->deliver(msg);
    }
}

void Server::handle_request(const nlohmann::json& req, std::shared_ptr<Session> session) {
    std::string action = req.value("action", "");

    if (action == "get") {
        nlohmann::json resp;
        resp["type"] = "list";
        resp["items"] = nlohmann::json::array();
        for (auto& it : todo_manager_.list()) resp["items"].push_back(it.to_json());
        session->deliver(resp.dump() + "\n");
    }
    else if (action == "add") {
        auto desc = req.value("description", "");
        if (!desc.empty()) {
            auto item = todo_manager_.add(desc);
            nlohmann::json notif{ {"type","added"},{"item",item.to_json()} };
            broadcast(notif.dump() + "\n");
        }
    }
    else if (action == "toggle") {
        int id = req.value("id", -1);
        auto updated = todo_manager_.toggle(id);
        if (updated) {
            nlohmann::json notif{ {"type","updated"},{"item", updated->to_json()} };
            broadcast(notif.dump() + "\n");
        }
        else {
            nlohmann::json resp{ {"type","error"},{"message","Item not found"} };
            session->deliver(resp.dump() + "\n");
        }
    }
    else {
        nlohmann::json resp{ {"type","error"},{"message","Item not found"} };
        session->deliver(resp.dump() + "\n");
    }
}

void Server::do_accept() {
    acceptor_.async_accept([this](asio::error_code ec, asio::ip::tcp::socket socket) {
        if (!ec) {
            auto session = std::make_shared<Session>(std::move(socket), *this);
            join(session);
            session->start();
        }
        do_accept();
        });
}
