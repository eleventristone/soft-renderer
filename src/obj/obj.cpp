#include "../../include/obj/obj.hpp"
#include <sstream>
#include <iostream>
#include <fstream>

void Obj::Parse(std::string raw) {
    std::istringstream stream(raw);
    std::string line;

    while (std::getline(stream, line)) {
        if (line[0] == 'v') {
            line.erase(0, 2);
            std::istringstream lineStream(line);
            std::string numStr;
            while (lineStream >> numStr) {
                try {
                    double num = std::stod(numStr);
                    vertices.push_back(num);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "invalid number: " << numStr << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << "number out of range: " << numStr << std::endl;
                }
            }
        }
        if (line[0] == 'f') {
            line.erase(0, 2);
            std::istringstream lineStream(line);
            std::string numStr;
            while (lineStream >> numStr) {
                try {
                    // int num = std::stoi(numStr) - 1;
                    int num = std::stoi(numStr.substr(0, numStr.find("/"))) - 1; // 带有法线贴图等情况时，f中每个元素为 a/b/c 的形式，这里仅取顶点索引
                    indices.push_back(num);
                } catch (const std::invalid_argument& e) {
                    std::cerr << "invalid number: " << numStr << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << "number out of range: " << numStr << std::endl;
                }
            }
        }
    }

    std::cout << "size of vertices: " << vertices.size() << std::endl;
    std::cout << "size of indices: " << indices.size() << std::endl;
}

void Obj::Save(std::string path) {
    FILE* file = fopen(path.c_str(), "w");
    size_t vertexN = vertices.size();
    size_t indexN = indices.size();

    for (int i = 0; i < vertexN; i += 3) {
        fprintf(file, "v %.8lf %.8lf %.8lf\n", vertices[i], vertices[i + 1], vertices[i + 2]);
    }

    for (int i = 0; i < indexN; i += 3) {
        fprintf(file, "f %d %d %d\n", indices[i] + 1, indices[i + 1] + 1, indices[i + 2] + 1);
    }
}