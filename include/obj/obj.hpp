#include <vector>
#include <string>

class Obj {
   public:
    std::vector<int> indices;
    std::vector<float> vertices;

    void Parse(std::string raw);
    void Save(std::string path);
};