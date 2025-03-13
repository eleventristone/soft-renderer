#include <vector>
#include <string>

std::string ReadFile(std::string path);

// std::vector<unsigned char> ReadFile(std::string path);

void WriteFile(std::string path, const char* data);

void WriteFile(std::string path, std::vector<unsigned char> data);