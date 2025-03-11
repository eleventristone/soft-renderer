#include "../../include/tools/file.h"
#include <fstream>
#include <iostream>
#include <vector>

std::string ReadFile(std::string path) {
    std::ifstream fin(path, std::ios::binary);
    std::string s((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

    return s;
}

// std::vector<unsigned char> ReadFile(std::string path) {
//     // std::vector<std::byte> v;
//     std::ifstream fin(path, std::ios::in);
//     std::vector<unsigned char> v((std::istreambuf_iterator<char>(fin)), std::istreambuf_iterator<char>());

//     return v;
// }

void WriteFile(std::string path, const char* data) {
    std::ofstream fout(path, std::ios::out);

    if (fout.is_open()) {
        fout << data;
        fout.close();
    }
}

void WriteFile(std::string path, std::vector<unsigned char> data) {
    std::ofstream fout(path, std::ios::out);

    if (fout.is_open()) {
        for (auto b : data) {
            fout << b;
        }
        fout.close();
    }
}