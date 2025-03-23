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

constexpr float left = -1.;
constexpr float right = 1.;
constexpr float top = 1.;
constexpr float bottom = -1.;
constexpr float near = -10.;
constexpr float far = 10.;

constexpr float fov = 120.;

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

// 模型变换：将对象从自己的局部坐标系转换到世界坐标系的变换(local -> world)
Matrixf model() {
    // TODO ...

    return Matrixf{
        4,
        4,
        {1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1}};
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

// 投影变换：将视图坐标系中的三维点映射到裁剪空间(clip space)，再经过透视除法，转换到NDC（如果是正交投影，则不需要透视除法)
Matrixf projection(bool perspective = true) {
    // float aspectRatio = width / height;
    // float alpha =
    
    if (perspective) {
        return Matrixf{
            4,
            4,
            {2 * near / (right - left), 0, (right + left) / (right - left), 0,
             0, 2 * near / (top - bottom), (top + bottom) / (top - bottom), 0,
             0, 0, -(far + near) / (far - near), -2 * far * near / (far - near),
             0, 0, -1, 0}};
    } else {  // orthographic
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
                          0, 0, 2 / (near - far), 0,  // near的z值比far要大，因为+z轴的方向是从far到near的
                          0, 0, 0, 1}};

        // 先平移再缩放
        return scale * translation;

        // return Matrixf{
        //     4,
        //     4,
        //     {2 / (right - left), 0, 0, -(right + left) / (right - left),
        //      0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
        //      0, 0, -2 / (far - near), -(far + near) / (far - near),
        //      0, 0, 0, 1}};
    }
}

// 视口变换：从NDC(-1 ~ 1)到屏幕空间坐标(0 ~ width/height/1)
Vector3f toScreen(const Vector3f& input) {
    return Vector3f(int((input.x + 1.) * width / 2. + .5), int((input.y + 1.) * height / 2. + .5), (input.z + 1.) / 2);
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

    float area = (triangle[0].x * (triangle[1].y - triangle[2].y) + triangle[1].x * (triangle[2].y - triangle[0].y) + triangle[2].x * (triangle[0].y - triangle[1].y));  // 这里不除以2，因为下面计算子三角形，叉乘时也不除以2，这样直接做除法运算就行

    // if (area < 1e-6) {
    //     return;
    // }

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            // 向量叉乘性质，结果正负值相同说明在同一侧，即内侧
            float pab = triangle[0].x * (triangle[1].y - y) + triangle[1].x * (y - triangle[0].y) + x * (triangle[0].y - triangle[1].y);  // Spab = AB x AP / 2
            float pbc = triangle[1].x * (triangle[2].y - y) + triangle[2].x * (y - triangle[1].y) + x * (triangle[1].y - triangle[2].y);  // Spbc = BC x BP / 2
            float pac = triangle[2].x * (triangle[0].y - y) + triangle[0].x * (y - triangle[2].y) + x * (triangle[2].y - triangle[0].y);  // Spac = CA x CP / 2

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
    float pos[3] = {0.5f, 0.5f, 3.0f};
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

    Matrixf mat = projection(false) * view(camera);
    // Matrixf mat = projection(false);

    std::cout << "mat: \n"
              << mat << std::endl;

    for (int i = 0; i < obj.indices.size(); i += 3) {
        Matrixf p1 = mat * (Matrixf{4, 1, {obj.vertices[obj.indices[i] * 3], obj.vertices[obj.indices[i] * 3 + 1], obj.vertices[obj.indices[i] * 3 + 2], 1}});
        Matrixf p2 = mat * (Matrixf{4, 1, {obj.vertices[obj.indices[i + 1] * 3], obj.vertices[obj.indices[i + 1] * 3 + 1], obj.vertices[obj.indices[i + 1] * 3 + 2], 1}});
        Matrixf p3 = mat * (Matrixf{4, 1, {obj.vertices[obj.indices[i + 2] * 3], obj.vertices[obj.indices[i + 2] * 3 + 1], obj.vertices[obj.indices[i + 2] * 3 + 2], 1}});

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