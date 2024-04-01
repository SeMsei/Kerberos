#pragma once

#include "functions.h"

#include <bitset>
#include <map>
#include <string>

class SS {
    static inline std::bitset<64> Ktgs_ss = std::bitset<64>(123);

    static inline std::bitset<64> Kc_ss = std::bitset<64>(0);

    static inline std::bitset<64> ss = std::bitset<64>(123);
    static inline std::bitset<64> t4 = std::bitset<64>(0);

public:
    static int send(std::map<std::string, std::bitset<64>> mp);

    static std::bitset<64> recv();

    static void print_keys();
};
