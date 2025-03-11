
struct Point3d {
    union {
        float x, y, z;
        float coords[3];
    };

    Point3d(float x, float y, float z)
        : coords{x, y, z} {};

    Point3d(float* coords)
        : coords{coords[0], coords[1], coords[2]} {};
};