#pragma once
#include <vector>
#include <string>
#include <bitset>
#include <map>
#include <chrono>

#include "functions.h"+

class KDC {
    static inline std::vector<std::wstring> logins = {L"123", L"12", L"ЯЯЯЯ", L"ЕГОР", L"ABCD", L"АБВГ"};
    static inline std::wstring login = L"";
    static inline std::bitset<64> Kc_tgs = std::bitset<64>(123);
    static inline std::bitset<64> Kas_tgs = std::bitset<64>(123);
    static inline std::bitset<64> Kc = std::bitset<64>(123);
    static inline std::bitset<64> tgs = std::bitset<64>(123);

    static inline std::bitset<64> Kc_ss = std::bitset<64>(123);
    static inline std::bitset<64> Ktgs_ss = std::bitset<64>(123);
    static inline std::bitset<64> ss = std::bitset<64>(123);

    static inline std::bitset<64> id = std::bitset<64>(0);

    static const int p1 = 5;
    static const int p2 = 5;
public:

    static bool send_login(std::wstring log);

    static std::map<std::string, std::bitset<64>> recv_first();

    static int send_to_tgs(std::map<std::string, std::bitset<64>> in);

    static std::map<std::string, std::bitset<64>> recv_tgs();

    static void print_keys();
};
