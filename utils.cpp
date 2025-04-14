#include "utils.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <conio.h>
#include <openssl/evp.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>


std::string sha256(const std::string& input) {
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sha256();
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, input.c_str(), input.size());
    EVP_DigestFinal_ex(mdctx, hash, &hash_len);
    EVP_MD_CTX_free(mdctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < hash_len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

std::string Get_Password() {
    std::string password;
    char ch;
    while ((ch = _getch()) != '\r') {
        if (ch == '\b') {
            if (!password.empty()) {
                std::cout << "\b \b";
                password.pop_back();
            }
        }
        else {
            password.push_back(ch);
            std::cout << '*';
        }
    }
    std::cout << std::endl;
    return password;
}

int prov(std::string s) {
    std::string x = s;
    int dots = 0;
    bool numbers = 0, letters = 0, symbols = 0, def = 0;
    while (!s.empty() && s[0] == ' ') s.erase(s.begin());
    while (!s.empty() && s.back() == ' ') s.pop_back();
    if (s.empty() || s[0] == '.' || s.back() == '.') return -1;

    for (char c : s) {
        if (c == '.') dots++;
        else if (c == '-') def = 1;
        else if (isdigit(c)) numbers = 1;
        else if (isalpha(c)) letters = 1;
        else symbols = 1;
    }

    if (dots == 1 && !letters && !symbols && numbers && !def && x.size() < 9) return 2;
    else if (!letters && !dots && numbers && !symbols && !def && x.size() < 9) return 1;
    else if (!dots && !symbols && !numbers && s.back() != '-') return 0;
    else return -1;
}

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
