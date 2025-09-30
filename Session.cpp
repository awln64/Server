#include "Session.hpp"
#include "IRequestHandler.hpp"
#include <iostream>

Session::Session(tcp::socket socket, IRequestHandler& handler)
    : socket_(std::move(socket)), handler_(handler) {
}

void Session::start() {
    do_read();
}

void Session::deliver(const std::string& msg) {
    bool write_in_progress = false;
    {
        std::lock_guard<std::mutex> lk(write_mutex_);
        write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
    }
    if (!write_in_progress) do_write();
}

void Session::do_read() {
    auto self(shared_from_this());
    asio::async_read_until(socket_, streambuf_, '\n',
        [this, self](asio::error_code ec, std::size_t length) {
            if (!ec) {
                std::istream is(&streambuf_);
                std::string line;
                std::getline(is, line);
                if (!line.empty()) {
                    try {
                        auto j = json::parse(line);
                        handler_.handle_request(j, self);
                    }
                    catch (std::exception& e) {
                        deliver(json{ {"type","error"}, {"message", std::string("Bad JSON: ") + e.what()} }.dump() + "\n");
                    }
                }
                do_read();
            }
            else {
                handler_.leave(self);
                asio::error_code ignored;
                socket_.close(ignored);
            }
        });
}

void Session::do_write() {
    auto self = shared_from_this();

    std::lock_guard<std::mutex> lk(write_mutex_);
    if (write_msgs_.empty()) return;

    const std::string& msg = write_msgs_.front();

    asio::async_write(socket_, asio::buffer(msg.data(), msg.size()),
        [this, self](asio::error_code ec, std::size_t /*length*/) {
            bool has_more = false;
            {
                std::lock_guard<std::mutex> lk2(write_mutex_);
                if (!ec) {
                    write_msgs_.pop_front();
                    has_more = !write_msgs_.empty();
                }
            }

            if (!ec) {
                if (has_more) {
                    do_write();
                }
            }
            else {
                handler_.leave(self);
                asio::error_code ignored;
                socket_.close(ignored);
            }
        });
}
