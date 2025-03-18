#include <vector>
#include <iostream>

class Matrixf {
   public:
    int row, col;
    std::vector<float> data;

    Matrixf(int row, int col, float fill = 0.0);
    Matrixf(int row, int col, std::vector<float> data);

    const float& operator()(int row, int col) const;
    float& operator()(int row, int col);
    const float& operator()(int row) const;
    float& operator()(int row);
    Matrixf operator+(const Matrixf& m) const;
    Matrixf operator-(const Matrixf& m) const;
    Matrixf operator*(const Matrixf& m) const;
    friend std::ostream& operator<<(std::ostream& os, const Matrixf& vec);

    void Scale(const float& factor);
    Matrixf Transpose() const;
    Matrixf Inverse() const;
    float Det() const;
};

// class Matrixd {};