#include "kdc.h"
#include <iostream>

bool KDC::send_login(std::wstring log) {

    if (std::find(logins.begin(), logins.end(), log) == logins.end()) {
        return false;
    } else {
        login = log;
        return true;
    }
}

std::map<std::string, std::bitset<64>> KDC::recv_first() {
    std::map<std::string, std::bitset<64>> ret;

    //std::cout << "\n\n" << Kc_tgs << "\n\n";
    ret["Kc_tgs"] = des_encrypt(Kc_tgs, Kc);
    //std::cout << "\n\n" << ret["Kc_tgs"] << "\n\n";
    auto encr_c = des_encrypt(wstring_to_bitset(login), Kas_tgs);
    auto encr_tgs = des_encrypt(tgs, Kas_tgs);

    auto t1 = std::chrono::system_clock::now();
    auto t1_int = std::chrono::duration_cast<std::chrono::seconds>(t1.time_since_epoch()).count();

    auto encr_t1 = des_encrypt(std::bitset<64>(t1_int)/*time to bitset*/, Kas_tgs);
    auto encr_p1 = des_encrypt(std::bitset<64>(p1)/*p1 to bitset*/, Kas_tgs);
    auto encr_Kc_tgs = des_encrypt(Kc_tgs, Kas_tgs);

    ret["c"] = des_encrypt(encr_c, Kc);
    ret["tgs"] = des_encrypt(encr_tgs, Kc);
    ret["t1"] = des_encrypt(encr_t1, Kc);
    ret["p1"] = des_encrypt(encr_p1, Kc);
    ret["Kc_tgs_in_tgt"] = des_encrypt(encr_Kc_tgs, Kc);

    return ret;

}

int KDC::send_to_tgs(std::map<std::string, std::bitset<64>> in) {
    std::bitset<64> Kc_tgs_in_tgt = des_decrypt(in["Kc_tgs_in_tgt"], Kas_tgs);
    //std::cout << "Kc_tgs_in_tgt: " << Kc_tgs_in_tgt << '\n';
    std::bitset<64> log = des_decrypt(in["c"], Kas_tgs);
    std::bitset<64> tgs_in = des_decrypt(in["tgs"], Kas_tgs);
    std::bitset<64> t1 = des_decrypt(in["t1"], Kas_tgs);
    std::bitset<64> p1 = des_decrypt(in["p1"], Kas_tgs);
    std::bitset<64> log_in_aut1 = des_decrypt(in["c_in_aut1"], Kc_tgs);
    std::bitset<64> t2 = des_decrypt(in["t2"], Kc_tgs);
    id = in["ID"];


    std::cout << "\nlog: " << log << '\n';
    std::cout << "\nlog in aut: " << log_in_aut1 << '\n';

    if (log_in_aut1 != log)
        return 1;

    if (tgs_in != tgs)
        return 2;

    if (t1.to_ullong() + p1.to_ullong() < t2.to_ullong())
        return 3;

    if (Kc_tgs_in_tgt != Kc_tgs)
        return 4;

    return 0;
}

std::map<std::string, std::bitset<64>> KDC::recv_tgs() {
    std::map<std::string, std::bitset<64>> ret;

    auto encr_c = des_encrypt(wstring_to_bitset(login), Ktgs_ss);
    auto encr_ss = des_encrypt(ss, Ktgs_ss);
    auto t3 = std::chrono::system_clock::now();
    auto t3_int = std::chrono::duration_cast<std::chrono::seconds>(t3.time_since_epoch()).count();

    auto encr_t3 = des_encrypt(std::bitset<64>(t3_int)/*time to bitset*/, Ktgs_ss);
    auto encr_p2 = des_encrypt(std::bitset<64>(p2)/*p1 to bitset*/, Ktgs_ss);
    auto encr_Kc_ss = des_encrypt(Kc_ss, Ktgs_ss);

    ret["c"] = des_encrypt(encr_c, Kc_tgs);
    ret["ss"] = des_encrypt(encr_ss, Kc_tgs);
    ret["t3"] = des_encrypt(encr_t3, Kc_tgs);
    ret["p2"] = des_encrypt(encr_p2, Kc_tgs);
    ret["Kc_ss_in_tgs"] = des_encrypt(encr_Kc_ss, Kc_tgs);
    ret["Kc_ss"] = des_encrypt(Kc_ss, Kc_tgs);

    return ret;
}

void KDC::print_keys() {
    std::cout << "\nkeys: ";
    std::cout << "Kc_tgs " << Kc_tgs << '\n';
    std::cout << "Kas_tgs " << Kas_tgs << '\n';
    std::cout << "Kc " << Kc << '\n';

    std::cout << "Kc_ss " << Kc_ss << '\n';
    std::cout << "Ktgs_ss " << Ktgs_ss << '\n';
}
