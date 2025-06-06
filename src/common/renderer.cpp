#include "renderer.h"
#include <random>
#include "fractal.h"
#include "color_mode.h"

void render_block(
    uint8_t* buffer,
    const ImageSettings& image_settings,
    const FractalSettings& fractal_settings,
    const Camera& camera,
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height)
{

    FractalSampler* fractal_sampler = get_fractal_sampler(fractal_settings.type);
    ColorFunction* color_function = get_color_function(fractal_settings.color_mode);

    double pixel_size_x = 1.0 / image_settings.width;
    double pixel_size_y = 1.0 / image_settings.height;
    double aspect_ratio = (double)image_settings.width / (double)image_settings.height;

    uint32_t n_samples = sqrt(image_settings.multi_sample_anti_aliasing);

    // Computes the color for each subpixel of the partial image
    for (uint32_t j = 0; j < height; ++j) {
        // Computes pixel Y coordinate [0, height - 1]
        uint32_t pixel_y = image_settings.height - 1 - y - j;

        for (uint32_t i = 0; i < width; ++i) {

            // Computes pixel X coordinate [0, width - 1]
            uint32_t pixel_x = x + i;

            float r = 0.0f, g = 0.0f, b = 0.0f;

            // Adds multi sample anti aliasing
            for (uint32_t sx = 0; sx < n_samples; sx++) {
                double sample_offset_x = (double)sx / n_samples;
                double sample_x = pixel_x + pixel_size_x * sample_offset_x;

                for (uint32_t sy = 0; sy < n_samples; sy++) {

                    // Random offset in [0, 1)
                    double sample_offset_y = (double)sy / n_samples;
                    double sample_y = pixel_y + pixel_size_y * sample_offset_y;

                    // Computes normalized coordinates in range [-0.5, 0.5]
                    double nx = ((double)sample_x / image_settings.width - 0.5) * aspect_ratio;
                    double ny = ((double)sample_y / image_settings.height - 0.5);

                    // Computes world coordinates with camera
                    number wx, wy;
                    camera.to_world(nx, ny, wx, wy);
                    float t = fractal_sampler(wx, wy, fractal_settings);

                    // Clamps t in range [0.0, 1.0]
                    if (t < 0.0f)
                        t = 0.0f;

                    else if (t > 1.0f)
                        t = 1.0f;

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
            }

            // Stores color into buffer by averaging the colors and mapping to [0, 255]
            uint32_t idx = (j * width + i) * 3;
            buffer[idx] = r / image_settings.multi_sample_anti_aliasing * 255;
            buffer[idx + 1] = g / image_settings.multi_sample_anti_aliasing * 255;
            buffer[idx + 2] = b / image_settings.multi_sample_anti_aliasing * 255;
        }
    }
}
