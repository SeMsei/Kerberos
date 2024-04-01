#include "functions.h"

namespace std {
template<>
void swap<bitset<64>::reference>(bitset<64>::reference &lhs, bitset<64>::reference &rhs) noexcept(is_nothrow_move_constructible_v<std::bitset<64>::reference> && is_nothrow_move_assignable_v<std::bitset<64>::reference>) {
    bool temp = (bool)lhs;
    lhs = (bool)rhs;
    rhs = (bool)temp;
}
}

void key_shift(std::bitset<28> &key, int step) {
    static std::vector<int>shift_val = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

    int shift = shift_val[step];

    if (shift == 1) {
        auto tmp = key[27];

        key <<= shift;
        key[0] = tmp;
    } else {
        auto tmp1 = key[26], tmp2 = key[27];

        key <<= shift;
        key[0] = tmp1;
        key[27] = tmp2;
    }
}

std::bitset<56> pc1(const std::bitset<64> &s) {
    static std::vector<int> perm{57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26,
                                 18, 10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52,
                                 44, 36,
                                 63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30,
                                 22, 14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4};

    std::bitset<56> out;

    for (int i = 0; i < 56; i++) {
        /*if (i % 8 == 0 && i != 0) [[unlikely]] {
            continue;
        } else [[likely]] {
            tmp[i] = s[perm[i]];
        }*/

        out[i] = s[perm[i] - 1];
    }

    return out;
}

std::bitset<48> pc2(const std::bitset<56> &s) {
    static std::vector<int> perm{14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10,
                                 23, 19, 12, 4, 26, 8, 16, 7, 27, 20, 13, 2,
                                 41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
                                 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32};

    std::bitset<48> out;

    for (int i = 0; i < 48; i++) {
        out[i] = s[perm[i] - 1];
    }

    return out;
}

template<int N>
void split_bitset(const std::bitset<N*2> &main,
                  std::bitset<N> &left, std::bitset<N> &right) {
    std::bitset<2*N> div(4294967295);

    left = std::bitset<N> ((main & div).to_ullong());
    right = std::bitset<N>(((main >> N) & div).to_ullong());
}

template<int N>
std::bitset<2*N> merge_bitset(const std::bitset<N> &l, const std::bitset<N> &r) {
    std::bitset<2*N> out;

    for (int i = 0; i < N; i++) {
        out[i] = l[i];
    }

    for (int i = 0; i < N; i++) {
        out[i + N] = r[i];
    }

    return out;
}

std::bitset<48> expand(const std::bitset<32> &r) {
    std::bitset<48> out;

    out[0] = r[31];
    out[47] = r[0];

    for (int i = 0; i < 8; i++) {
        if (i != 0)
            out[i * 6] = r[i * 4 -1];
        if (i != 7)
            out[i * 6 + 5] = r[(i + 1) * 4];

        for (int j = 1; j < 5; j++) {
            out[i * 6 + j] = r[i * 4 + j - 1];
        }
    }

    return out;
}

std::bitset<32> reduce(const std::bitset<48> &s) {
    std::vector<std::vector<std::vector<int>>> s_boxes  = {{{14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
                                                           {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
                                                           {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
                                                           {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},

                                                          {{15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
                                                           {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
                                                           {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
                                                           {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},

                                                          {{10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
                                                           {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
                                                           {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
                                                           {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},

                                                          {{7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
                                                           {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
                                                           {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
                                                           {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},

                                                          {{2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
                                                           {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
                                                           {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
                                                           {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},

                                                          {{12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
                                                           {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
                                                           {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
                                                           {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},

                                                          {{4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
                                                           {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
                                                           {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
                                                           {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},

                                                          {{13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
                                                           {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
                                                           {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
                                                           {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

    std::bitset<32> out;

    std::bitset<48> div(0b111111);

    for (int i = 0; i < 8; i++) {
        std::bitset<6> tmp(((s >> 6*i) & div).to_ulong());

        std::bitset<6> col_div(0b1111);
        std::bitset<4> col(((tmp >> 1) & col_div).to_ulong());


        std::bitset<4> new_val(s_boxes[i][tmp[0] * 2 + tmp[5]][col.to_ulong()]);

        for (int j = i * 4; j < (i+1) * 4; j++) {
            out[j] = new_val[j - i * 4];
        }

    }

    return out;
}

void p_perm(std::bitset<32> &s) {
    std::vector<int> p = {16, 7, 20, 21, 29, 12, 28, 17, 1, 15, 23, 26, 5, 18,
                          31, 10, 2, 8, 24, 14, 32, 27, 3, 9, 19, 13, 30, 6, 22,
                          11, 4, 25};

    std::bitset<32> tmp;

    for (int i = 0; i < 32; i++) {
        tmp[i] = s[p[i] - 1];
    }

    s = tmp;
}

void init_perm(std::bitset<64> &set) {
    static std::vector<int> perm{58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
                                 62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
                                 57, 49, 41, 33, 25, 17, 9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
                                 61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7};

    std::bitset<64> tmp;

    for (int i = 0; i < 64; i++) {
        tmp[i] = set[perm[i] - 1];
    }

    set = tmp;
}

void final_perm(std::bitset<64> &set) {
    static std::vector<int> perm{40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
                                 38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
                                 36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
                                 34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9, 49, 17, 57, 25};

    std::bitset<64> tmp;

    for (int i = 0; i < 64; i++) {
        tmp[i] = set[perm[i] - 1];
    }

    set = tmp;
}

void des_step(std::bitset<64> &set, std::bitset<56> &key, int step) {
    std::bitset<32> left, right;
    split_bitset<32>(set, left, right);

}

std::bitset<64> wstring_to_bitset(const std::wstring &s) {
    std::bitset<64> out;
    std::bitset<16> tmp;

    for (int i = 0; i < s.size(); i++) {
        tmp = std::bitset<16>((wint_t)s[i]);

        for (int j = 0; j < 16; j++) {
            out[j + 16 * i] = tmp[j];
        }
    }

    return out;
}

std::wstring bitset_to_wstring(const std::bitset<64> &s) {
    std::wstring out;
    std::bitset<16> tmp;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 16; j++) {
            tmp[j] = s[j + i * 16];
        }

        if (tmp.to_ullong() == 0)
            break;

        out += tmp.to_ullong();
    }

    return out;
}

std::bitset<64> des_encrypt(std::bitset<64> txt, std::bitset<64> key) {
    init_perm(txt);

    std::bitset<32> l, r;
    split_bitset<32>(txt, l, r);

    auto k1 = pc1(key);

    std::bitset<28> c, d;
    split_bitset<28>(k1, c, d);

    for (int step = 0; step < 16; step++) {

        key_shift(c, step);
        key_shift(d, step);

        auto tmp_l = l;
        l = r;
        auto exp_r = expand(r);
        auto c_d = pc2(merge_bitset<28>(c, d));
        //keys.push_back(c_d);
        exp_r ^= c_d;

        auto box = reduce(exp_r);

        p_perm(box);

        r = tmp_l ^ box;
    }

    auto l_r = merge_bitset<32>(l, r);

    final_perm(l_r);

    //std::wcout << "encrypted: " << l_r << '\n';

    return l_r;
}

void generate_keys(std::vector<std::bitset<48>> &keys, const std::bitset<64> &key) {
    auto k1 = pc1(key);
    //std::vector<std::bitset<48>> ret;

    std::bitset<28> c, d;
    split_bitset<28>(k1, c, d);

    for (int step = 0; step < 16; step++) {

        key_shift(c, step);
        key_shift(d, step);

        //auto tmp_l = l;
        //l = r;
        //auto exp_r = expand(r);
        auto c_d = pc2(merge_bitset<28>(c, d));
        keys.push_back(c_d);
        //exp_r ^= c_d;

        //auto box = reduce(exp_r);

        //p_perm(box);

        //r = tmp_l ^ box;
    }

    //auto l_r = merge_bitset<32>(l, r);

    //final_perm(l_r);

    //std::wcout << "encrypted: " << l_r << '\n';

    //return ret;
}

std::bitset<64> des_decrypt(std::bitset<64> encr, std::bitset<64> key) {
    std::bitset<32> l, r;

    std::vector<std::bitset<48>> keys;
    generate_keys(keys, key);

    split_bitset<32>(encr, l, r);
    init_perm(encr);
    split_bitset<32>(encr, l ,r);

    for (int step = 0; step < 16; step++) {
        auto tmp_r = r;
        r = l;

        auto x = expand(l);
        x ^= keys[15 - step];
        auto box = reduce(x);

        p_perm(box);

        l = tmp_r ^ box;
    }

    auto f_l_r = merge_bitset<32>(l, r);
    final_perm(f_l_r);

    return f_l_r;
}


