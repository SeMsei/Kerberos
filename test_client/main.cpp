#include <iostream>
#include <boost/asio.hpp>
#include "windows.h"
#include <map>
#include <bitset>
#include "json.hpp"
#include "functions.h"



std::bitset<64> Kc(123);
std::bitset<64> Kc_tgs;
std::bitset<64> Kc_ss;
std::map<std::string, unsigned long long> mp;
std::map<std::string, std::bitset<64>> mp1, mp2;
std::wstring login;

void tgs(boost::asio::ip::tcp::socket &socket, boost::asio::ip::tcp::resolver &resolver) {
    boost::asio::connect(socket, resolver.resolve("127.0.0.1", "25000"));

    //third step
    std::cout << "\n!!!!!!!!third step!!!!!!!!!\n";
    std::map<std::string, std::bitset<64>> in;
    in["c"] = des_decrypt(mp1["c"], Kc);
    in["tgs"] = des_decrypt(mp1["tgs"], Kc);
    in["t1"] = des_decrypt(mp1["t1"], Kc);
    in["p1"] = des_decrypt(mp1["p1"], Kc);
    in["Kc_tgs_in_tgt"] = des_decrypt( mp1["Kc_tgs_in_tgt"], Kc);
    in["c_in_aut1"] = des_encrypt(wstring_to_bitset(login), Kc_tgs);
    auto t2 = std::chrono::system_clock::now();
    auto t2_int = std::chrono::duration_cast<std::chrono::seconds>(t2.time_since_epoch()).count();
    in["t2"] = des_encrypt(std::bitset<64>(t2_int), Kc_tgs);
    in["ID"] = std::bitset<64>(0); //num of ss server

    std::cout << "\ndata sent: ";

    nlohmann::json jd1;

    for (auto x:in) {
        std::cout << x.first << ' ' << x.second << '\n';
        //
        jd1[x.first] = x.second.to_string();
    }

    jd1["type"] = "tgs";

    std::cout << "dump: " << jd1.dump();

    auto result1 = boost::asio::write(socket, boost::asio::buffer(jd1.dump() + '\0'));
    std::cout << result1 << "/" << jd1.dump().size() << " was sent\n";

    //fourth step
    std::cout << "\n!!!!!!!!fourth step!!!!!!!!!\n";

    boost::asio::streambuf resp1;
    resp1.prepare(1024);
    boost::asio::read_until(socket, resp1, '\0');


    std::string resp_data1 {std::istreambuf_iterator<char>(&resp1), std::istreambuf_iterator<char>()};
    auto f_js = nlohmann::json::parse(resp_data1);

    if (f_js["status"] == "err") {
        std::cout << f_js["msg"];
        return;
    }

    std::cout << "\ndata recv: " << resp_data1 << '\n';

    std::cout << "\ndata recv in bits: \n";
    for (auto x:f_js.items()) {
        if (x.key() == "status" || x.key() == "msg")
            continue;

        //
        std::cout << x.key() << ' ' << bitset_from_bitstring(x.value()) << '\n';
        //
        mp2[x.key()] = std::bitset<64>((std::string)x.value());
    }

    //auto mp1 = KDC::recv_tgs();
    //
    Kc_ss = des_decrypt(std::bitset<64>((std::string)f_js["Kc_ss"]), Kc);

    std::cout << "\nrecvd Kc_ss: " << Kc_ss.to_ullong() << '\n';

    // and close the connection now
    boost::system::error_code ec;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    //socket.close();
}

void ss(boost::asio::ip::tcp::socket &socket, boost::asio::ip::tcp::resolver &resolver) {
    //SS SERVER
    boost::asio::connect(socket, resolver.resolve("127.0.0.1", "25001"));

    //fifth_step
    std::cout << "\n!!!!!!!!fifth step!!!!!!!!!\n";
    std::map<std::string, std::bitset<64>> in;
    //in.clear();
    in["c"] = des_decrypt(mp2["c"], Kc_tgs);
    in["ss"] = des_decrypt(mp2["ss"], Kc_tgs);
    in["t3"] = des_decrypt(mp2["t3"], Kc_tgs);
    in["p2"] = des_decrypt(mp2["p2"], Kc_tgs);
    in["Kc_ss_in_tgs"] = des_decrypt(mp2["Kc_ss_in_tgs"], Kc_tgs);

    in["c_in_aut2"] = des_encrypt(wstring_to_bitset(login), Kc_ss);//wstring

    std::cout << "\ndata sent: ";
    auto t4 = std::chrono::system_clock::now();
    auto t4_int = std::chrono::duration_cast<std::chrono::seconds>(t4.time_since_epoch()).count();
    in["t4"] = des_encrypt(std::bitset<64>(t4_int), Kc_ss);


    nlohmann::json jd2;

    for (auto x:in) {
        std::cout << x.first << ' ' << x.second << '\n';
        jd2[x.first] = x.second.to_string();
    }

    jd2["type"] = "ss";

    auto result2 = boost::asio::write(socket, boost::asio::buffer(jd2.dump()));
    std::cout << result2 << "/" << jd2.dump().size() << " was sent\n";

    //sixth step
    std::cout << "\n!!!!!!!!sixth step!!!!!!!!!\n";
    boost::asio::streambuf resp2;
    resp2.prepare(1024);
    boost::asio::read_until(socket, resp2, '\0');


    std::string resp_data2 {std::istreambuf_iterator<char>(&resp2), std::istreambuf_iterator<char>()};
    auto s_js = nlohmann::json::parse(resp_data2);

    if (s_js["status"] == "err") {
        std::cout << s_js["msg"] << '\n';
        return;
    }

    //auto t4_ss = s_js["time"];

    std::cout << "recvd data: \n";

    for (auto x:s_js.items()) {
        std::cout << x.key() << ' ' << x.value() << '\n';
    }

    std::bitset<64> t4_bs = std::bitset<64>((std::string)s_js["time"]);

    /*for (int i = 0; i < 64; i++) {
        t4_bs[i] = t4_ss[63 - i];
    }*/

    std::cout << des_decrypt(t4_bs, Kc_ss).to_ullong() << ' ' << t4_int << '\n';

    if (des_decrypt(t4_bs, Kc_ss).to_ullong() - 1 == t4_int)
        std::cout << "It's real SS server\n";
    else
        std::cout << "Someone try to fool u\n";

    boost::system::error_code ec;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Rus");
#ifdef _WIN32
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
#endif
    using boost::asio::ip::tcp;

    boost::asio::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve("127.0.0.1", "25000"));

    //first step

    std::cout << "\n!!!!!!!!first_step!!!!!!!!!\n";

    std::cout << "Enter ur login: ";
    std::wcin >> login;
    std::wcout << login + L" login was sent\n";

    //mp["c"] = wstring_to_bitset(login).to_ullong();
    //mp["type"] = wstring_to_bitset(L"as").to_ullong();
    //nlohmann::json jd = mp;
    nlohmann::json jd;
    jd["c"] = wstring_to_bitset(login).to_string();
    jd["type"] = wstring_to_bitset(L"as").to_string();

    for (auto x: jd.items())
        std::cout << x.key() << ' ' << x.value() << '\n';

    auto result = boost::asio::write(socket, boost::asio::buffer(jd.dump()));
    std::cout << result << "/" << jd.dump().size() << " was sent\n";


    //second step
    std::cout << "\n!!!!!!!!second step!!!!!!!!!\n";

    boost::asio::streambuf resp;
    boost::asio::read_until(socket, resp, '\0');


    std::string resp_data {std::istreambuf_iterator<char>(&resp), std::istreambuf_iterator<char>()};
    auto second_js = nlohmann::json::parse(resp_data);

    if (second_js["status"] == "err") {
        std::cout << second_js["msg"];
        return 0;
    }

    std::cout << "\ndata recv: " << resp_data << '\n';

    std::cout << "\ndata recv in bits: \n";
    for (auto x:second_js.items()) {
        if (x.key() == "status" || x.key() == "msg")
            continue;

        std::cout << x.key() << ' ' << bitset_from_bitstring(x.value()) << '\n';
        //mp1[x.key()] = std::bitset<64>(x.value());
        mp1[x.key()] = std::bitset<64>((std::string)x.value());
    }


    //
    Kc_tgs = des_decrypt(std::bitset<64>((std::string)second_js["Kc_tgs"]), Kc);

    std::cout << "\nrecvd Ks_tgs: " << Kc_tgs.to_ullong() << '\n';

    boost::system::error_code ec;
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);

    while (1) {
        std::string opt;
        std::cout << "enter server to connect: [tgs/ss]";

        std::cin >> opt;

        if (opt == "ss") {
            ss(socket, resolver);
        } else if (opt == "tgs") {
            tgs(socket, resolver);
        } else {
            std::cout << "U don't deserve to use it. BB!\n";
            break;
        }
    }

    socket.close();

    return 0;
}
