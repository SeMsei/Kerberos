#pragma once
#include <bitset>
#include <vector>
#include <string>
#include <map>

void key_shift(std::bitset<28> &key, int step);

std::bitset<56> pc1(const std::bitset<64> &s);

std::bitset<48> pc2(const std::bitset<56> &s);

template<int N>
void split_bitset(const std::bitset<N*2> &main,
                  std::bitset<N> &left, std::bitset<N> &right);

template<int N>
std::bitset<2*N> merge_bitset(const std::bitset<N> &l, const std::bitset<N> &r);

std::bitset<48> expand(const std::bitset<32> &r);

std::bitset<32> reduce(const std::bitset<48> &s);

void p_perm(std::bitset<32> &s);

void init_perm(std::bitset<64> &set);

void final_perm(std::bitset<64> &set);

void des_step(std::bitset<64> &set, std::bitset<56> &key, int step);

std::bitset<64> wstring_to_bitset(const std::wstring &s);

std::wstring bitset_to_wstring(const std::bitset<64> &s);

std::bitset<64> des_encrypt(std::bitset<64> txt, std::bitset<64> key);

void generate_keys(std::vector<std::bitset<48>> &keys, const std::bitset<64> &key);

std::bitset<64> des_decrypt(std::bitset<64> encr, std::bitset<64> key);
