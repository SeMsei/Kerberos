#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio.hpp>
#include "json.hpp"
#include <bitset>
#include "kdc.h"
#include <future>
#include "SS.h"

class session : public std::enable_shared_from_this<session>
{
public:
    session(boost::asio::ip::tcp::socket socket)
        : m_socket(std::move(socket)) { }
    void run() {
        m_buffer.prepare(1024);
        wait_for_request();
    }
private:
    void wait_for_request() {
        auto self(shared_from_this());
        boost::asio::async_read_until(m_socket, m_buffer, "\0", [this, self](boost::system::error_code ec, std::size_t length)
            {
                if (!ec)  {
                nlohmann::json jd_out;


                    std::string data{ std::istreambuf_iterator<char>(&m_buffer),
                                                                std::istreambuf_iterator<char>()
                                                               };

                std::cout << "size: " <<  m_buffer.size() << '\n';
                    std::cout << "recvd data: " << data << '\n';
                    nlohmann::json jd = nlohmann::json::parse(data);
                    std::cout << "recvd data: " << data << '\n';


                    if (jd["type"] == wstring_to_bitset(L"as").to_string()) {
                        std::bitset<64> log = std::bitset<64>((std::string)jd.at("c"));

                        std::wcout << "\n\nrecvd login: " << bitset_to_wstring(log) << '\n';

                        if (KDC::send_login(bitset_to_wstring(log))) {
                            std::wcout << "login was found!\n";

                            auto ret = KDC::recv_first();
                            jd_out["status"] = "ok";

                            for (auto x:ret) {
                                jd_out[x.first] = x.second.to_string();
                            }

                        } else {
                            std::wcout << "login was not found!\n";

                            jd_out["status"] = "err";
                            jd_out["msg"] = "No suh login";
                        }

                    } else if (jd["type"] == "tgs") {
                        std::cout << "\n\ntgs recv: \n";

                        for (auto x:jd.items()) {
                            std::cout << x.key() << ' ' << x.value() << '\n';
                        }

                        std::map<std::string, std::bitset<64>> mp;

                        for (auto x: jd.items()) {
                            if (x.key() != "type") [[likely]] {
                                mp[x.key()] = std::bitset<64>((std::string)x.value());
                            }
                        }

                        int tgs_res = KDC::send_to_tgs(mp);

                        if (tgs_res == 0) [[likely]] {
                            std::cout << "Cool!!\n";

                            jd_out["status"] = "ok";

                            auto tgs_resp = KDC::recv_tgs();

                            for (auto x:tgs_resp) {
                                //
                                jd_out[x.first] = x.second.to_string();
                            }
                        } else if (tgs_res == 1) {
                            std::cout << "smth went wrong!\n";

                            jd_out["status"] = "err";
                            jd_out["msg"] = "different login in aut and tgt";
                        } else if (tgs_res == 2) {
                            std::cout << "smth went wrong!\n";

                            jd_out["status"] = "err";
                            jd_out["msg"] = "different tgs in req and server";
                        } else if (tgs_res == 3) {
                            std::cout << "smth went wrong!\n";

                            jd_out["status"] = "err";
                            jd_out["msg"] = "req is outdated";
                        } else if (tgs_res == 4) {
                            std::cout << "smth went wrong!\n";

                            jd_out["status"] = "err";
                            jd_out["msg"] = "different Kc_tgs in tgt and req";
                        }
                    } else {
                        std::cout << "Oh no, wrong req type:(\n";

                        jd_out["status"] = "err";
                        jd_out["msg"] = "Wrong req type";
                    }

                    send_buf = jd_out.dump() + '\0';

                    send_response();

                    /*boost::asio::async_write(m_socket, boost::asio::buffer(send_buf),
                                             [this, self](boost::system::error_code ec, std::size_t /*length)
                                             {


                                                 std::cout << send_buf << '\n';
                                                 if (!ec) {

                                                 } else {
                                                     std::cout << "Error sending response: " << ec << std::endl;
                                                 }
                                             });*/

                    wait_for_request();
                } else {
                    std::cout << "error: " << ec << std::endl;
                }

                //wait_for_request();

            });
    }

private:

    void send_response() {
        auto self(shared_from_this());
        std::cout << "buf to send: " << send_buf << "\n\n\n\n";
        boost::asio::async_write(m_socket, boost::asio::buffer(send_buf),
            [this, self](boost::system::error_code ec, std::size_t /*length*/)
            {
            std::cout << ec << '\n';
                if (!ec) {
                    //wait_for_request();
                } else {
                    std::cout << "Error sending response: " << ec << std::endl;
                }
        });
    }
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::streambuf m_buffer;
    std::string send_buf;
};

class server
{
public:
    server(boost::asio::io_context& io_context, short port)
        : m_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
        KDC::print_keys();

        do_accept();
    }
private:
    void do_accept() {
        m_acceptor.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
            if (!ec) {
                std::cout << "creating session on: "
                          << socket.remote_endpoint().address().to_string()
                          << ":" << socket.remote_endpoint().port() << '\n';
                std::make_shared<session>(std::move(socket))->run();
            } else {
                std::cout << "error: " << ec.message() << std::endl;
            }
            do_accept();
        });
    }
private:
    boost::asio::ip::tcp::acceptor m_acceptor;
};



int main(int argc, char* argv[])
{
    boost::asio::io_context io_context;
    server s(io_context, 25000);
    io_context.run();

    return 0;
}
