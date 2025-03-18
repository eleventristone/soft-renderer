#include "../../include/geometry/matrix.hpp"

Matrixf _identity(int size) {
    Matrixf result(size, size);
    for (int i = 0; i < size; ++i) {
        result(i, i) = 1.0;
    }
    return result;
}

Matrixf::Matrixf(int row, int col, float fill)
    : row(row), col(col), data(std::vector<float>(row * col, fill)) {}

Matrixf::Matrixf(int row, int col, std::vector<float> data)
    : row(row), col(col), data(data) {}

const float& Matrixf::operator()(int row, int col) const {
    return data[row * this->col + col];
}

float& Matrixf::operator()(int row, int col) {
    return data[row * this->col + col];
}

const float& Matrixf::operator()(int row) const {
    return data[row * this->col];
}

float& Matrixf::operator()(int row) {
    return data[row * this->col];
}

Matrixf Matrixf::operator+(const Matrixf& m) const {
    if (row != m.row || col != m.col) {
        throw std::invalid_argument("Matrix dimensions do not match for addition");
    }

    Matrixf result(row, col);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            result(i, j) = (*this)(i, j) + m(i, j);
        }
    }
    return result;
}

Matrixf Matrixf::operator-(const Matrixf& m) const {
    if (row != m.row || col != m.col) {
        throw std::invalid_argument("Matrix dimensions do not match for subtraction");
    }

    Matrixf result(row, col);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            auto ttt = result(i, j);
            result(i, j) = (*this)(i, j) - m(i, j);
        }
    }
    return result;
}

Matrixf Matrixf::operator*(const Matrixf& m) const {
    if (col != m.row) {
        throw std::invalid_argument("Matrix dimensions do not match for multiplication");
    }

    Matrixf result(row, m.col);
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < m.col; ++j) {
            for (int k = 0; k < col; ++k) {
                result(i, j) += (*this)(i, k) * m(k, j);
            }
        }
    }
    return result;
}

std::ostream& operator<<(std::ostream& os, const Matrixf& mat) {
    os.precision(30);
    for (int i = 0; i < mat.row; i++) {
        for (int j = 0; j < mat.col; j++) {
            os << mat(i, j) << " ";
        }
        os << std::endl;
    }

    return os;
}

void Matrixf::Scale(const float& factor) {
    for (int i = 0; i < data.size(); i++) {
        data[i] *= factor;
    }
}

Matrixf Matrixf::Transpose() const {
    Matrixf result(col, row);
    for (int i = 0; i < row; ++i) {
        for (int j = 0; j < col; ++j) {
            result(j, i) = (*this)(i, j);
        }
    }
    return result;
}

Matrixf Matrixf::Inverse() const {
    if (row != col) {
        throw std::invalid_argument("Inverse is defined only for square matrices");
    }

    if (this->Det() == 0) {
        throw std::invalid_argument("The matrix can not be inversed");
    }

    int order = row;  // 阶数

    Matrixf tempMat(*this);
    Matrixf augmentedMat = _identity(order);  // 增广矩阵

    // 高斯-约旦消元法
    // 变换成对角矩阵（通过行初等变换，包括缩放某行、加减某行的c倍、交换两行）
    for (int i = 0; i < order; i++) {
        // 先确认本行在对角线上的元素是否为0，若为0则需要从其他行加上来
        if (tempMat(i, i) == 0) {
            for (int j = 0; j < order; j++) {
                if (tempMat(j, i) != 0) {
                    for (int k = 0; k < order; k++) {
                        tempMat(i, k) += tempMat(j, k);
                        augmentedMat(i, k) += augmentedMat(j, k);
                    }
                    break;
                }
            }
        }

        // 将同列的其他元素变为0
        for (int j = 0; j < order; j++) {
            if (j == i) {
                continue;  // 跳过第i行
            }
            if (tempMat(j, i) != 0) {
                float scale = tempMat(j, i) / tempMat(i, i);
                for (int k = 0; k < order; k++) {  // 行初等变换-加减
                    tempMat(j, k) -= tempMat(i, k) * scale;
                    augmentedMat(j, k) -= augmentedMat(i, k) * scale;
                }
            }
        }
    }

    // 变化成单位矩阵（行初等变换-缩放）
    for (int i = 0; i < order; i++) {
        if (tempMat(i, i) != 1) {
            float scale = 1 / tempMat(i, i);
            for (int j = 0; j < order; j++) {
                tempMat(i, j) *= scale;
                augmentedMat(i, j) *= scale;
            }
        }
    }

    return augmentedMat;
}

float Matrixf::Det() const {
    int order = row;  // 阶数

    if (order == 1) {
        return (*this)(0, 0);
    } else if (order == 2) {
        return (*this)(0, 0) * (*this)(1, 1) - (*this)(0, 1) * (*this)(1, 0);
    }

    // 基于代数余子式递归展开求和，参考https://zhuanlan.zhihu.com/p/266228209
    float result = 0.0;
    for (int z = 0; z < order; z++) {
        Matrixf cofactor(order - 1, order - 1);
        for (int i = 1; i < order; i++) {  // 基于第一行展开（假设为行优先存储），所以这里i=1，即直接从第二行开始
            int curCol = 0;
            for (int j = 0; j < order; j++) {
                if (j == z) {
                    continue;  // 跳过Aij所在的列
                }
                cofactor(i - 1, curCol) = (*this)(i, j);
                curCol++;
            }
        }
        result += (*this)(0, z) * cofactor.Det() * (z % 2 == 0 ? 1 : -1);
    }

    return result;
}