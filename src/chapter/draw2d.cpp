// #include <iostream>

// // #define STB_IMAGE_IMPLEMENTATION
// // #include "../third_party/stb_image/stb_image.h"

// #include <cmath>
// #include "../third_party/tgaimage/tgaimage.h"
// #include "../src/tools/file.h"
// #include "../src/obj/obj.h"

// constexpr TGAColor white = {255, 255, 255, 255};  // attention, BGRA order
// constexpr TGAColor green = {0, 255, 0, 255};
// constexpr TGAColor red = {0, 0, 255, 255};
// constexpr TGAColor blue = {255, 128, 64, 255};
// constexpr TGAColor yellow = {0, 200, 255, 255};

// TGAColor randColor() {
//     return {
//         static_cast<unsigned char>(std::rand() % 255),
//         static_cast<unsigned char>(std::rand() % 255),
//         static_cast<unsigned char>(std::rand() % 255),
//         static_cast<unsigned char>(std::rand() % 255)};
// }

// void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color) {
//     bool reversed = false;
//     float r = (by - ay) / static_cast<float>(bx - ax);
//     // 斜率绝对值大于1时，需要交换x和y，避免绘制时出现间隙
//     if (std::abs(r) > 1) {
//         std::swap(ax, ay);
//         std::swap(bx, by);
//         reversed = true;
//     }

//     if (ax > bx) {
//         std::swap(ax, bx);
//         std::swap(ay, by);
//     }

//     float ratio = (by - ay) / static_cast<float>(bx - ax);
//     float addition = (ax * by - bx * ay) / static_cast<float>(ax - bx);

//     for (int x = ax; x <= bx; x++) {
//         int y = ratio * x + addition;
//         if (reversed) {
//             framebuffer.set(y, x, color);
//         } else {
//             framebuffer.set(x, y, color);
//         }
//     }
// }

// void drawLine() {
//     constexpr int width = 64;
//     constexpr int height = 64;
//     TGAImage framebuffer(width, height, TGAImage::RGB);

//     int ax = 7, ay = 3;
//     int bx = 12, by = 37;
//     int cx = 62, cy = 53;

//     line(ax, ay, bx, by, framebuffer, blue);
//     line(cx, cy, bx, by, framebuffer, green);
//     line(cx, cy, ax, ay, framebuffer, yellow);
//     line(ax, ay, cx, cy, framebuffer, red);

//     framebuffer.set(ax, ay, white);
//     framebuffer.set(bx, by, white);
//     framebuffer.set(cx, cy, white);

//     framebuffer.write_tga_file("../temp/framebuffer.tga");
// }

// // 基于扫描线的三角面绘制
// void triangleWithScanline(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) {
//     // bubble sort
//     if (ax > bx) {
//         std::swap(ax, bx);
//         std::swap(ay, by);
//     }
//     if (ax > cx) {
//         std::swap(ax, cx);
//         std::swap(ay, cy);
//     }
//     if (bx > cx) {
//         std::swap(bx, cx);
//         std::swap(by, cy);
//     }

//     float ratioAB = (by - ay) / static_cast<float>(bx - ax);
//     float additionAB = (ax * by - bx * ay) / static_cast<float>(ax - bx);

//     float ratioBC = (cy - by) / static_cast<float>(cx - bx);
//     float additionBC = (bx * cy - cx * by) / static_cast<float>(bx - cx);

//     float ratioAC = (cy - ay) / static_cast<float>(cx - ax);
//     float additionAC = (ax * cy - cx * ay) / static_cast<float>(ax - cx);

//     // 竖扫描线绘制
//     for (int x = ax; x <= cx; x++) {
//         if (x < bx) {
//             line(x, ratioAB * x + additionAB, x, ratioAC * x + additionAC, framebuffer, color);
//             framebuffer.set(x, ratioAB * x + additionAB, white);
//             framebuffer.set(x, ratioAC * x + additionAC, white);
//         } else {
//             line(x, ratioBC * x + additionBC, x, ratioAC * x + additionAC, framebuffer, color);
//             framebuffer.set(x, ratioBC * x + additionBC, white);
//             framebuffer.set(x, ratioAC * x + additionAC, white);
//         }
//     }
// }

// void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) {
//     int minX = std::min(cx, std::min(ax, bx));
//     int minY = std::min(cy, std::min(ay, by));
//     int maxX = std::max(cx, std::max(ax, bx));
//     int maxY = std::max(cy, std::max(ay, by));

//     // float area = 1 / 2 * (ax * (by - cy) + bx * (cy - ay) + cx * (ay - by));

//     for (int x = minX; x <= maxX; x++) {
//         for (int y = minY; y <= maxY; y++) {
//             // 向量叉乘性质，结果正负值相同说明在同一侧，即内侧
//             float ABxAP = ax * (by - y) + bx * (y - ay) + x * (ay - by);
//             float BCxBP = bx * (cy - y) + cx * (y - by) + x * (by - cy);
//             float CAxCP = cx * (ay - y) + ax * (y - cy) + x * (cy - ay);

//             // 两面绘制
//             // if ((ABxAP <= 0 && BCxBP <= 0 && CAxCP <= 0) || (ABxAP >= 0 && BCxBP >= 0 && CAxCP >= 0)) {
//             //     framebuffer.set(x, y, color);
//             // }

//             // 只绘制正面（默认逆时针为正方向）
//             if (ABxAP <= 0 && BCxBP <= 0 && CAxCP <= 0) {
//                 framebuffer.set(x, y, color);
//             }
//         }
//     }
// }

// void drawTriangle() {
//     constexpr int width = 128;
//     constexpr int height = 128;
//     TGAImage framebuffer(width, height, TGAImage::RGB);

//     // triangleWithScanline(7, 45, 35, 100, 45, 60, framebuffer, red);
//     // triangleWithScanline(120, 35, 90, 5, 45, 110, framebuffer, white);
//     // triangleWithScanline(115, 83, 80, 90, 85, 120, framebuffer, green);

//     triangle(7, 45, 35, 100, 45, 60, framebuffer, red);
//     triangle(120, 35, 90, 5, 45, 110, framebuffer, white);
//     triangle(115, 83, 80, 90, 85, 120, framebuffer, green);

//     framebuffer.write_tga_file("../temp/framebuffer.tga");
// }