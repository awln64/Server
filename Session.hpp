#pragma once
#include <asio.hpp>
#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include "json.hpp"

using asio::ip::tcp;
using json = nlohmann::json;

class IRequestHandler;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, IRequestHandler& handler);
    void start();
    void deliver(const std::string& msg);

private:
    void do_read();
    void do_write();

    tcp::socket socket_;
    IRequestHandler& handler_;
    asio::streambuf streambuf_;
    std::deque<std::string> write_msgs_;
    std::mutex write_mutex_;
};
