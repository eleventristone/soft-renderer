
#include <vector>
#include <ostream>

// struct Vector3f {
//     union {
//         struct {
//             float x;
//             float y;
//             float z;
//         };
//         float coord[3];
//     };

//     Vector3f(float x, float y, float z)
//         : coord{x, y, z} {};

//     Vector3f(float* xyz)
//         : coord{xyz[0], xyz[1], xyz[2]} {};
// };

class Vector3f {
   public:
    union {
        struct {
            float x;
            float y;
            float z;
        };
        float coords[3];
    };

    Vector3f(float fiil = 0.0);
    Vector3f(float x, float y, float z);
    Vector3f(float* xyz);
    // Vector3f(std::vector<float> point);

    Vector3f operator+(const Vector3f& v) const;
    Vector3f operator-(const Vector3f& v) const;
    // bool operator<(const Vector3f& v) const;
    bool operator==(const Vector3f& v) const;
    friend std::ostream& operator<<(std::ostream& os, const Vector3f& vec);

    float Dot(const Vector3f& v) const;
    Vector3f Cross(const Vector3f& v) const;
    void Normalize();
    void Scale(const float& factor);
    float Length() const;
};
