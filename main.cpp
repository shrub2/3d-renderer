#include <cmath>
#include "tgaimage.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(float ax, float ay, float bx, float by, TGAImage &framebuffer, TGAColor color) {
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if (steep) { // if the line is steep, we transpose the image
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax>bx) { // make it left−to−right
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    float y = ay;
    for (float x=ax; x<=bx; x++) {
        if (steep) // if transposed, de−transpose
            framebuffer.set(y, x, color);
        else
            framebuffer.set(x, y, color);
        y += (by-ay) / static_cast<float>(bx-ax);
    }
}

struct Vertex {
    float x, y, z;
};

struct Face {
    int a, b, c;
};

int main(int argc, char** argv) {
    constexpr int width  = 128;
    constexpr int height = 128;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    std::ifstream file("./obj/diablo3_pose/diablo3_pose.obj");

    if (!file.is_open()) {
        std::cerr << "Failed to open file\n";
        return 1;
    }

    std::string line;
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    while (std::getline(file, line)) {
        if (line[0] == 'v' && line[1] == ' ') {
            std::istringstream iss(line);
            char prefix;
            Vertex v;
            iss >> prefix >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (line[0] == 'f' && line[1] == ' ') {
            std::istringstream iss(line);
            char prefix;
            iss >> prefix;

            Face f;
            std::string token;
            int* targets[] = { &f.a, &f.b, &f.c };

            for (int i = 0; i < 3; i++) {
                iss >> token;
                std::istringstream ts(token);
                std::string num;
                std::getline(ts, num, '/');
                *targets[i] = std::stoi(num) - 1;
            }
            faces.push_back(f);
        }
    }
    file.close();

    for (Face f : faces) {
        Vertex va = vertices[f.a];
        Vertex vb = vertices[f.b];
        Vertex vc = vertices[f.c];
        line(va.x, va.y, vb.x, vb.y, framebuffer, red);
        line(vb.x, vb.y, vc.x, vc.y, framebuffer, red);
        line(vc.x, vc.y, va.x, va.y, framebuffer, red);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

