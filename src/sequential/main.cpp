#include <stdint.h>
#include <chrono>
#include <vector>
#include "common/common.h"
#include "common/renderer.h"
#include <memory>
#include "common/output_handler.h"
#include "common/logging.h"

int main(int argc, char** argv)
{

    Settings settings;

    bool run_program = load_args(argc, argv, settings);

    if (!run_program) {
        return 0;
    }

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

    LOG_STATUS("Computation took: " << duration.count() << " ms");

    // Creates output handler based on the settings mode
    std::shared_ptr<OutputHandler> output_handler = OutputHandler::factory_create(settings.output_settings.mode);

    bool success = output_handler->save_output(
        buffer,
        settings.image.width,
        settings.image.height,
        settings.output_settings);

    if (!success) {
        LOG_ERROR("Unable to output image...");
    }

    return 0;
}