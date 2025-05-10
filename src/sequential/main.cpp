#include <stdint.h>
#include <chrono>
#include <vector>
#include "common/common.h"
#include "common/renderer.h"
#include "common/image_utils.h"

int main(int argc, char** argv)
{

    Settings settings;

    load_args(argc, argv, settings);

    std::chrono::time_point start = std::chrono::high_resolution_clock::now();

    std::vector<uint8_t> buffer(settings.image.width * settings.image.height * 3);
    render_block(
        buffer,
        settings.image,
        settings.fractal,
        settings.camera,
        0,
        0,
        settings.image.width,
        settings.image.height);

    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Computation took: " << duration.count() << " ms\n";
    save_image(
        settings.output_path,
        buffer.data(),
        settings.image.width,
        settings.image.height);

    return 0;
}