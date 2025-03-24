#include "../../include/geometry/vector.hpp"
#include <cmath>
#include <stdexcept>
#include <iostream>

Vector3f::Vector3f(float fill)
    : coords{fill, fill, fill} {};

Vector3f::Vector3f(float x, float y, float z)
    : coords{x, y, z} {};

Vector3f::Vector3f(float* xyz)
    : coords{xyz[0], xyz[1], xyz[2]} {};

// Vector3f::Vector3f(std::vector<float> point)
//     : coords(point) {};

Vector3f Vector3f::operator+(const Vector3f& v) const {
    Vector3f result(0.0f);
    result.x = coords[0] + v.coords[0];
    result.y = coords[1] + v.coords[1];
    result.z = coords[2] + v.coords[2];

    return result;
}

Vector3f Vector3f::operator-(const Vector3f& v) const {
    Vector3f result(0.0);
    result.coords[0] = coords[0] - v.coords[0];
    result.coords[1] = coords[1] - v.coords[1];
    result.coords[2] = coords[2] - v.coords[2];

    return result;
}

// bool Vector3f::operator<(const Vector3f& v) const {
//     if (coords[0] != v.coords[0]) {
//         return coords[0] < v.coords[0];
//     }
//     if (coords[1] != v.coords[1]) {
//         return coords[1] < v.coords[1];
//     }

//     return coords[2] < v.coords[2];
// }

bool Vector3f::operator==(const Vector3f& v) const {
    return coords[0] == v.coords[0] && coords[1] == v.coords[1] && coords[2] == v.coords[2];
}

std::ostream& operator<<(std::ostream& os, const Vector3f& vec) {
    os << "[" << vec.coords[0] << ", " << vec.coords[1] << ", " << vec.coords[2] << "]";

    return os;
}

float Vector3f::Dot(const Vector3f& v) const {
    return coords[0] * v.coords[0] + coords[1] * v.coords[1] + coords[2] * v.coords[2];
}

Vector3f Vector3f::Cross(const Vector3f& v) const {
    return Vector3f(coords[1] * v.coords[2] - coords[2] * v.coords[1], coords[2] * v.coords[0] - coords[0] * v.coords[2], coords[0] * v.coords[1] - coords[1] * v.coords[0]);
}

void Vector3f::Normalize() {
    float length = Length();
    // std::cout << "length: " << length << std::endl;
    for (int i = 0; i < 3; i++) {
        coords[i] /= length;
    }
}

void Vector3f::Scale(const float& factor) {
    for (int i = 0; i < 3; i++) {
        coords[i] *= factor;
    }
}

float Vector3f::Length() const {
    auto len = sqrt(coords[0] * coords[0] + coords[1] * coords[1] + coords[2] * coords[2]);
    if (len > 1e8) {
        std::cout << "length: " << len << std::endl;
    }
    return len;
}