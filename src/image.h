#pragma once

struct Image {
    uint8_t* pixels;
    int width;
    int height;
    int channels;

    Image(const std::string& path, bool flip = true);
    ~Image();
};