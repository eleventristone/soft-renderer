#include <iostream>

// #define STB_IMAGE_IMPLEMENTATION
// #include "../third_party/stb_image/stb_image.h"

#include <cmath>
#include <vector>
#include "../third_party/tgaimage/tgaimage.h"
#include "../include/tools/file.hpp"
#include "../include/obj/obj.hpp"
#include "../include/geometry/vec.hpp"

constexpr TGAColor white = {255, 255, 255, 255};  // attention, BGRA order
constexpr TGAColor green = {0, 255, 0, 255};
constexpr TGAColor red = {0, 0, 255, 255};
constexpr TGAColor blue = {255, 128, 64, 255};
constexpr TGAColor yellow = {0, 200, 255, 255};

constexpr int width = 512;
constexpr int height = 512;

TGAColor randColor() {
    return {
        static_cast<unsigned char>(std::rand() % 255),
        static_cast<unsigned char>(std::rand() % 255),
        static_cast<unsigned char>(std::rand() % 255),
        static_cast<unsigned char>(std::rand() % 255)};
}

void triangle(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz, TGAImage& framebuffer, TGAColor color, int* zBuffer) {
    int minX = std::min(cx, std::min(ax, bx)) + 0.5;
    int minY = std::min(cy, std::min(ay, by)) + 0.5;
    int maxX = std::max(cx, std::max(ax, bx)) + 0.5;
    int maxY = std::max(cy, std::max(ay, by)) + 0.5;

    float area = (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));  // 这里不除以2，因为下面计算子三角形，叉乘时也不除以2，这样直接做除法运算就行

    for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
            // 向量叉乘性质，结果正负值相同说明在同一侧，即内侧
            float pab = ax * (by - y) + bx * (y - ay) + x * (ay - by);  // Spab = AB x AP / 2
            float pbc = bx * (cy - y) + cx * (y - by) + x * (by - cy);  // Spbc = BC x BP / 2
            float pac = cx * (ay - y) + ax * (y - cy) + x * (cy - ay);  // Spac = CA x CP / 2

            // 利用重心坐标法计算z，即三角形内部一点P必定能写成 P=αA+βB+γC 的形式，且 α+β+γ=1，计算投影平面上三角形的α β γ值后，再使用权重乘以三个顶点的z值，获得一个插值出来的z值（虽然和实际坐标的z值未必一致，但是足以表达深度值depth）
            float z = pbc / area * az + pac / area * bz + pab / area * cz;

            // 只绘制正面（默认逆时针为正方向）
            if (pab >= 0 && pbc >= 0 && pac >= 0) {
                // std::cout << zBuffer[x * width + y] << " | " << z << std::endl;
                if (zBuffer[x * height + y] < z) {
                    continue;
                } else {
                    framebuffer.set(x, y, color);
                    zBuffer[x * height + y] = z;
                }
            }
        }
    }
}

void drawModel() {
    TGAImage framebuffer(width, height, TGAImage::RGB);

    Obj obj;
    obj.Parse(ReadFile("../model/african_head.obj"));

    int zBuffer[width * height];
    for (int i = 0; i < width * height; i++) {
        zBuffer[i] = INT_MAX;
    }

    // 仅取x,y坐标，通过变换到画布（从-1~+1变换成0~2，再变换成0~width/height）
    for (int i = 0; i < obj.indices.size(); i += 3) {
        unsigned char intensity = (1 - (obj.vertices[obj.indices[i] * 3 + 2] + obj.vertices[obj.indices[i + 1] * 3 + 2] + obj.vertices[obj.indices[i + 2] * 3 + 2]) / 3) * 255;  // 令近处强度更高（更接近白色）
        triangle((obj.vertices[obj.indices[i] * 3] + 1) / 2 * width,
                 (obj.vertices[obj.indices[i] * 3 + 1] + 1) / 2 * height,
                 obj.vertices[obj.indices[i] * 3 + 2],
                 (obj.vertices[obj.indices[i + 1] * 3] + 1) / 2 * width,
                 (obj.vertices[obj.indices[i + 1] * 3 + 1] + 1) / 2 * height,
                 obj.vertices[obj.indices[i + 1] * 3 + 2],
                 (obj.vertices[obj.indices[i + 2] * 3] + 1) / 2 * width,
                 (obj.vertices[obj.indices[i + 2] * 3 + 1] + 1) / 2 * height,
                 obj.vertices[obj.indices[i + 2] * 3 + 2],
                 framebuffer,
                 //  {intensity, intensity, intensity, 255},
                 randColor(),
                 zBuffer);
    }

    framebuffer.write_tga_file("../temp/framebuffer.tga");
}

class Camera {
   public:
    Vec3f position;
    Vec3f forward;
    Vec3f upward;

    Camera(float* pos, float* fwd, float* up)
        : position{pos}, forward{fwd}, upward{up} {}
};

Vec3f* projection(Vec3f a, Vec3f b, Vec3f c) {
    // TODO
}

void rasterization(std::vector<Vec3f> triangle, TGAImage& framebuffer, TGAColor color) {
    if (triangle.size() < 3) {
        std::cerr << "vertices less than 3." << std::endl;
        return;
    }
    
    // TODO 
}

int main(int argc, char** argv) {
    Camera camera((float[]){0.0f, 0.0f, 1.0f}, (float[]){0.0f, 0.0f, -1.0f}, (float[]){0.0f, 1.0f, 0.0f});

    drawModel();

    return 0;
}