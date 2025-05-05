#include "image_utils.h"

int main(int argc, char** argv)
{
    int width = 256;
    int height = 256;
    uint8_t* image = new uint8_t[width * height * 3];

    // Simple red-green gradient image
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = (y * width + x) * 3;
            image[idx + 0] = (float)x / (float)width * 255.0f; // R
            image[idx + 1] = (float)y / (float)height * 255.0f; // G
            image[idx + 2] = 0; // B
        }
    }

    save_image("image.png", image, width, height);

    delete[] image;

    return 0;
}
