#include <iostream>
#include <boost/asio/io_context.hpp>
#include <boost/asio.hpp>
#include "json.hpp"
#include <bitset>
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
        boost::asio::async_read_until(m_socket, m_buffer, "\0", [this, self](boost::system::error_code ec, std::size_t /*length*/)
                                      {
                                          if (!ec)  {
                                              nlohmann::json jd_out;

                                              std::string data{ std::istreambuf_iterator<char>(&m_buffer),
                                                  std::istreambuf_iterator<char>()
                                              };
                                              std::cout << "\n\nrecvd data: \n";
                                              std::cout << data << std::endl;

                                              nlohmann::json jd = nlohmann::json::parse(data);

                                              if (jd["type"] == "ss") {
                                                  std::map<std::string, std::bitset<64>> mp;

                                                  for (auto x: jd.items()) {
                                                      if (x.key() != "type") [[likely]] {
                                                          mp[x.key()] = std::bitset<64>((std::string)x.value());
                                                      }
                                                  }

                                                  auto ss_resp = SS::send(mp);

                                                  if (ss_resp == 0) [[likely]] {
                                                      std::cout << "all is ok\n";

                                                      jd_out["status"] = "ok";

                                                      auto time = SS::recv();

                                                      jd_out["time"] = time.to_string();
                                                  } else if (ss_resp == 1) {
                                                      std::cout << "Smth is wrong\n";

                                                      jd_out["status"] = "err";
                                                      jd_out["msg"] = "different login in aut and tgs";
                                                  } else if (ss_resp == 2) {
                                                      std::cout << "Smth is wrong\n";

                                                      jd_out["status"] = "err";
                                                      jd_out["msg"] = "different ss in req and server";
                                                  } else if (ss_resp == 3) {
                                                      std::cout << "Smth is wrong\n";

                                                      jd_out["status"] = "err";
                                                      jd_out["msg"] = "req is outdated";
                                                  }
                                              } else {
                                                  std::cout << "\n Oh no, wrong req type\n";

                                                  jd_out["status"] = "err";
                                                  jd_out["msg"] = "Wrong req type";
                                              }

                                              for (auto x:jd_out.items()) {
                                                  std::cout << x.key() << ' ' << x.value() << '\n';
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
        std::cout << "\nbuf to send: \n" << send_buf << '\n';
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
        SS::print_keys();

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
    server s(io_context, 25001);
    io_context.run();

    return 0;
}
