#include <iostream>

// #define STB_IMAGE_IMPLEMENTATION
// #include "../third_party/stb_image/stb_image.h"

#include <cmath>
#include <vector>
#include <chrono>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "../third_party/tgaimage/tgaimage.h"
#include "../include/tools/file.hpp"
#include "../include/obj/obj.hpp"
#include "../include/geometry/vector.hpp"
#include "../include/geometry/matrix.hpp"

constexpr TGAColor white = {255, 255, 255, 255};  // attention, BGRA order
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};

// 注意这些是基于相机坐标系的，left/right/top/bottom代表近裁剪面的左/右/上/下，透视投影时可以用fov/aspect替代
constexpr float left = -1.;
constexpr float right = 1.;
constexpr float top = 1.;
constexpr float bottom = -1.;
constexpr float near = -2;
constexpr float far = -4.;

// constexpr float fov = 120.;
// constexpr float aspect = 1.;

constexpr int width = 512;
constexpr int height = 512;

TGAColor randColor() {
    return {
        static_cast<unsigned char>(std::rand() % 255),
        static_cast<unsigned char>(std::rand() % 255),
        static_cast<unsigned char>(std::rand() % 255),
        255};
}

class Camera {
   public:
    Vector3f position;
    Vector3f forward;
    Vector3f upward;

    Camera(float* pos, float* fwd, float* up)
        : position{pos}, forward{fwd}, upward{up} {}
};

/*
    模型变换：将对象从自己的局部坐标系转换到世界坐标系的变换(local -> world)
    由于模型变换中，顺序会影响变换的最终结果，这里定义函数中变换顺序为缩放 -> 旋转 -> 平移（SRT）,旋转顺序为绕X -> Y -> Z，旋转方向默认逆时针，传入绕三个轴的旋转弧度
*/
Matrixf model(Vector3f s, Vector3f r, Vector3f t) {
    /*
        这里设定obj模型文件的坐标是基于世界坐标系的，因此不需要经过局部到世界的矩阵变换
        否则，如果平移、缩放、旋转变化是基于局部坐标系的，需要先进行变换后，再转换到世界坐标系；如果平移、缩放、旋转变化是基于世界坐标系的，需要先转换到世界坐标系后再进行变换
    */
    // Matrixf localToWorld = {};

    Matrixf scale = {4,
                     4,
                     {s.x, 0, 0, 0,
                      0, s.y, 0, 0,
                      0, 0, s.z, 0,
                      0, 0, 0, 1}};

    Matrixf rotateX = {4,
                       4,
                       {1, 0, 0, 0,
                        0, std::cos(r.x), -std::sin(r.x), 0,
                        0, std::sin(r.x), std::cos(r.x), 0,
                        0, 0, 0, 1}};

    Matrixf rotateY = {4,
                       4,
                       {std::cos(r.y), 0, std::sin(r.y), 0,
                        0, 1, 0, 0,
                        -std::sin(r.y), 0, std::cos(r.y), 0,
                        0, 0, 0, 1}};

    Matrixf rotateZ = {4,
                       4,
                       {std::cos(r.z), -std::sin(r.z), 0, 0,
                        std::sin(r.z), std::cos(r.z), 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1}};

    Matrixf translation = {4,
                           4,
                           {1, 0, 0, t.x,
                            0, 1, 0, t.y,
                            0, 0, 1, t.z,
                            0, 0, 0, 1}};

    return translation * rotateZ * rotateY * rotateX * scale;
}

// 视图变换/相机变换：将世界坐标系下的所有对象变换到观察者或相机的坐标系中(world -> camera)
Matrixf view(const Camera& camera) {
    Matrixf translation = Matrixf{
        4,
        4,
        {1, 0, 0, -camera.position.x,
         0, 1, 0, -camera.position.y,
         0, 0, 1, -camera.position.z,
         0, 0, 0, 1}};

    Vector3f nz = camera.forward;  // 相机的观察方向（对应相机-z轴）
    Vector3f y = camera.upward;    // 相机的上方向（对应相机y轴）
    Vector3f x = nz.Cross(y);      // 相机的右方向（对应相机x轴）

    nz.Normalize();
    y.Normalize();
    x.Normalize();

    Matrixf rotation = Matrixf{
        4,
        4,
        {x.x, y.x, -nz.x, 0,
         x.y, y.y, -nz.y, 0,
         x.z, y.z, -nz.z, 0,
         0, 0, 0, 1}};

    // 先平移再旋转
    return rotation * translation;
}

// 投影变换：将视图坐标系中的三维点映射到裁剪空间(clip space)，再变换到NDC（如果是透视投影，需要额外做透视除法，但透视除法需要除以未知数w，属于非线性变化，没法直接用矩阵表示)
Matrixf projection(bool perspective = true) {
    Matrixf perspToOrtho = {4,
                            4,
                            {-near, 0, 0, 0,
                             0, -near, 0, 0,
                             0, 0, -(near + far), near * far,
                             0, 0, -1, 0}};  // 这一步计算后的w=-z，因此后续需要做透视除法

    Matrixf translation = {4,
                           4,
                           {1, 0, 0, -(left + right) / 2,
                            0, 1, 0, -(top + bottom) / 2,
                            0, 0, 1, -(near + far) / 2,
                            0, 0, 0, 1}};

    Matrixf scale = {4,
                     4,
                     {2 / (right - left), 0, 0, 0,
                      0, 2 / (top - bottom), 0, 0,
                      0, 0, 2 / (near - far), 0,
                      0, 0, 0, 1}};

    Matrixf ortho = scale * translation;  // 先平移再缩放

    if (perspective) {
        return ortho * perspToOrtho;
    } else {
        return ortho;
    }
}

// 视口变换：从NDC(-1 ~ 1)到屏幕空间坐标(0 ~ width/height/1)
Vector3f toScreen(const Vector3f& input) {
    return Vector3f(int((input.x + 1.) * width / 2. + .5),
                    int((input.y + 1.) * height / 2. + .5),
                    (-input.z + 1.) / 2);  // 由于相机坐标系的观测方向为-z轴，因此近处的z值大，远处的z值小。但深度越小越靠前，因此需要把z值先取反
}

// 光栅化
void rasterization(std::vector<Vector3f> triangle, TGAImage& framebuffer, TGAColor color, float* zBuffer) {
    if (triangle.size() < 3) {
        std::cerr << "vertices less than 3." << std::endl;
        return;
    }

    // 传进来的已经是视口变换后的坐标，此时坐标值是float类型的int值
    float minX = std::max(0.f, std::min(triangle[2].x, std::min(triangle[0].x, triangle[1].x)));
    float minY = std::max(0.f, std::min(triangle[2].y, std::min(triangle[0].y, triangle[1].y)));
    float maxX = std::min(width + 0.f, std::max(triangle[2].x, std::max(triangle[0].x, triangle[1].x)));
    float maxY = std::min(height + 0.f, std::max(triangle[2].y, std::max(triangle[0].y, triangle[1].y)));

    float area = std::sqrt(triangle[0].x * (triangle[1].y - triangle[2].y) + triangle[1].x * (triangle[2].y - triangle[0].y) + triangle[2].x * (triangle[0].y - triangle[1].y)) / 2.;

    // if (area < 1e-6) {
    //     return;
    // }

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            // 向量叉乘性质，结果正负值相同说明在同一侧，即内侧
            float pab = std::sqrt(triangle[0].x * (triangle[1].y - y) + triangle[1].x * (y - triangle[0].y) + x * (triangle[0].y - triangle[1].y)) / 2.;  // Spab = |AB x AP| / 2
            float pbc = std::sqrt(triangle[1].x * (triangle[2].y - y) + triangle[2].x * (y - triangle[1].y) + x * (triangle[1].y - triangle[2].y)) / 2.;  // Spbc = |BC x BP| / 2
            float pac = std::sqrt(triangle[2].x * (triangle[0].y - y) + triangle[0].x * (y - triangle[2].y) + x * (triangle[2].y - triangle[0].y)) / 2.;  // Spac = |CA x CP| / 2

            // 利用重心坐标法计算z，即三角形内部一点P必定能写成 P=αA+βB+γC 的形式，且 α+β+γ=1，计算投影平面上三角形的α β γ值后，再使用权重乘以三个顶点的z值，获得一个插值出来的z值（虽然和实际坐标的z值未必一致，但是足以表达深度值depth）
            float z = pbc / area * triangle[0].z + pac / area * triangle[1].z + pab / area * triangle[2].z;

            if (z < 0 || z > 1 || z != z) {  // z!=z用于判断NaN
                continue;
            }

            // 只绘制正面（默认逆时针为正方向）
            if (pab >= 0 && pbc >= 0 && pac >= 0) {
                // std::cout << zBuffer[x * width + y] << " | " << z << std::endl;
                if (zBuffer[y * width + x] < z) {
                    continue;
                } else {
                    framebuffer.set(x, y, color);
                    zBuffer[y * width + x] = z;
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    float pos[3] = {0.f, 0.f, 3.0f};
    float fwd[3] = {0.0f, 0.0f, -1.0f};
    float up[3] = {0.0f, 1.0f, 0.0f};

    Camera camera(pos, fwd, up);

    TGAImage framebuffer(width, height, TGAImage::RGB);

    Obj obj;
    obj.Parse(ReadFile("../model/african_head.obj"));

    float zBuffer[width * height];
    for (int i = 0; i < width * height; i++) {
        zBuffer[i] = __FLT_MAX__;
    }

    Matrixf mat = projection(true) * view(camera) * model(Vector3f{.5, .5, .5}, Vector3f{M_PI / 4, M_PI / 4, M_PI / 4}, Vector3f{.5, .5, .5});

    std::cout << "mat: \n"
              << mat << std::endl;

    for (int i = 0; i < obj.indices.size(); i += 3) {
        Matrixf p1 = mat * (Matrixf{4, 1, {obj.vertices[obj.indices[i] * 3], obj.vertices[obj.indices[i] * 3 + 1], obj.vertices[obj.indices[i] * 3 + 2], 1}});
        Matrixf p2 = mat * (Matrixf{4, 1, {obj.vertices[obj.indices[i + 1] * 3], obj.vertices[obj.indices[i + 1] * 3 + 1], obj.vertices[obj.indices[i + 1] * 3 + 2], 1}});
        Matrixf p3 = mat * (Matrixf{4, 1, {obj.vertices[obj.indices[i + 2] * 3], obj.vertices[obj.indices[i + 2] * 3 + 1], obj.vertices[obj.indices[i + 2] * 3 + 2], 1}});
        // 透视除法
        p1.Scale(1 / p1(3));
        p2.Scale(1 / p2(3));
        p3.Scale(1 / p3(3));

        rasterization(
            std::vector<Vector3f>{
                toScreen(Vector3f{p1(0), p1(1), p1(2)}),
                toScreen(Vector3f{p2(0), p2(1), p2(2)}),
                toScreen(Vector3f{p3(0), p3(1), p3(2)})},
            framebuffer,
            randColor(),
            zBuffer);
    }

    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm buf;
    localtime_r(&now, &buf);
    std::ostringstream oss;
    oss << "../temp/framebuffer" << std::put_time(&buf, "%Y%m%d%H%M%S") << ".tga";

    framebuffer.write_tga_file(oss.str());

    return 0;
}