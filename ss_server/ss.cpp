#include "ss.h"
#include <iostream>

int SS::send(std::map<std::string, std::bitset<64>> mp) {
    //std::cout << "Kc_ss_in_tgs: " << mp["Kc_ss_in_tgs"] << '\n';
    Kc_ss = des_decrypt(mp["Kc_ss_in_tgs"], Ktgs_ss);
    //std::cout << "Kc_ss_in_tgs: " << Kc_ss_in_tgs << '\n';
    std::bitset<64> log = des_decrypt(mp["c"], Ktgs_ss);
    std::bitset<64> ss_in = des_decrypt(mp["ss"], Ktgs_ss);
    std::bitset<64> t3 = des_decrypt(mp["t3"], Ktgs_ss);
    std::bitset<64> p2 = des_decrypt(mp["p2"], Ktgs_ss);
    std::bitset<64> log_in_aut2 = des_decrypt(mp["c_in_aut2"], Kc_ss);
    t4 = des_decrypt(mp["t4"], Kc_ss);

    std::cout << "t4: " << t4 << "\nt3: " << t3 << '\n';

    if (log_in_aut2 != log)
        return 1;

    if (ss_in != ss)
        return 2;

    if (t3.to_ullong() + p2.to_ullong() < t4.to_ullong())
        return 3;

    return 0;
}

std::bitset<64> SS::recv() {
    std::bitset<64> ret;
    ret = des_encrypt(std::bitset<64>(t4.to_ullong() + 1), Kc_ss);
    std::cout << "ret: " << ret << '\n';
    return ret;
}

void SS::print_keys() {
    std::cout << "\nkeys: \n";

    std::cout << "Ktgs_ss " << Ktgs_ss << '\n';
    std::cout << "Kc_ss " << Kc_ss << '\n';
}
