
struct Vec3f {
    union {
        float x, y, z;
        float coord[3];
    };

    Vec3f(float x, float y, float z)
        : coord{x, y, z} {};

    Vec3f(float* xyz)
        : coord{xyz[0], xyz[1], xyz[2]} {};
};
