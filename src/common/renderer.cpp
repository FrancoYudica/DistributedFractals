#include "renderer.h"
#include <random>
#include "fractal.h"
#include "color_mode.h"

// Initialize the random number generator once
std::random_device rd;
std::mt19937 gen(rd());

// Uniform distribution between 0 and 1
std::uniform_real_distribution<> dist(0.0, 1.0);

void render_block(
    std::vector<uint8_t>& buffer,
    const ImageSettings& image_settings,
    const FractalSettings& fractal_settings,
    const Camera& camera,
    int x,
    int y,
    int width,
    int height)
{

    auto fractal_function = get_fractal_function(fractal_settings.type);
    auto color_function = get_color_function(fractal_settings.color_mode);

    double pixel_size_x = 1.0 / image_settings.width;
    double pixel_size_y = 1.0 / image_settings.height;
    double aspect_ratio = (double)image_settings.width / (double)image_settings.height;

    // Computes the color for each subpixel of the partial image
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {

            // Computes pixel coordinate [0, image_size - 1]
            int pixel_x = x + i;
            int pixel_y = y + j;

            float r = 0.0f, g = 0.0f, b = 0.0f;

            // Adds multi sample anti aliasing
            for (int sample = 1; sample <= image_settings.multi_sample_anti_aliasing; sample++) {

                // Random offset in [0, 1)
                double random_offset_x = dist(gen);
                double random_offset_y = dist(gen);

                double sample_x = pixel_x + pixel_size_x * random_offset_x;
                double sample_y = pixel_y + pixel_size_y * random_offset_y;

                // Computes normalized coordinates in range [-1.0, 1.0]
                double nx = ((double)sample_x / image_settings.width - 0.5) * 2.0 * aspect_ratio;
                double ny = ((double)sample_y / image_settings.height - 0.5) * 2.0;

                // Computes world coordinates with camera
                double wx, wy;
                camera.to_world(nx, ny, wx, wy);
                float t = fractal_function(wx, wy, fractal_settings);

                float sample_r, sample_g, sample_b;

                color_function(
                    t,
                    sample_r,
                    sample_g,
                    sample_b);

                r += sample_r;
                g += sample_g;
                b += sample_b;
            }

            // Stores color into buffer by averaging the colors and mapping to [0, 255]
            int idx = (j * width + i) * 3;
            buffer[idx] = r / image_settings.multi_sample_anti_aliasing * 255;
            buffer[idx + 1] = g / image_settings.multi_sample_anti_aliasing * 255;
            buffer[idx + 2] = b / image_settings.multi_sample_anti_aliasing * 255;
        }
    }
}