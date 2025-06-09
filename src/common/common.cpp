#include "common.h"
#include "common/logging.h"

void print_help()
{
    LOG("Fractal Renderer - Command Line Options");
    LOG("----------------------------------------");
    LOG("  -od, --output_disk       [opt filename]         Save output image to disk. Defaults to 'output.png' if no filename is provided.");
    LOG("  -on, --output_network    [opt IP [opt port]]    Send output image over TCP. Defaults to IP 0.0.0.0 and port 5001 if not specified.");
    LOG("  -w,  --width             <int>                  Image width in pixels");
    LOG("  -h,  --height            <int>                  Image height in pixels");
    LOG("  -s,  --samples           <int>                  Number of MSAA samples. Must be a perfect square number");
    LOG("  -b,  --block_size        <int>                  Size in pixels of the MPI image task");
    LOG("  -z,  --zoom              <float>                Zoom level of camera");
    LOG("  -cx, --camera_x          <float>                Camera X position");
    LOG("  -cy, --camera_y          <float>                Camera Y position");
    LOG("  -i,  --iterations        <int>                  Max iterations for fractal");
    LOG("  -t,  --type              <int>                  Fractal type ID");
    LOG("  --color_mode             <int>                  Color mode type ID");
    LOG("  --julia-cx               <float>                Real component of Julia set C constant");
    LOG("  --julia-cy               <float>                Imaginary component of Julia set C constant");
    LOG("  --quiet                                         Disables all console messages");
    LOG("  --help                                          Show this help message");
}

bool load_args(uint32_t argc, char** argv, Settings& settings)
{
    for (uint32_t arg_index = 1; arg_index < argc; ++arg_index) {
        const char* parameter = argv[arg_index];

        // Help command -----------------------------------------------------------------------
        if (!strcmp(parameter, "--help")) {
            print_help();
            return false;
        }

        // Quiet command ----------------------------------------------------------------------
        if (!strcmp(parameter, "--quiet")) {
            set_logging_enabled(false);
            _s_logging_verbose = false;
            continue;
        }
        // Arguments with multiple varying parameters -----------------------------------------
        if (!strcmp(parameter, "-od") || !strcmp(parameter, "--output_disk")) {
            settings.output_settings.mode = OutputSettingsMode::DISK;

            if (arg_index + 1 < argc)
                std::strcpy(settings.output_settings.disk_data.output_path, argv[++arg_index]);

            continue;
        }
        if (!strcmp(parameter, "-on") || !strcmp(parameter, "--output_network")) {
            settings.output_settings.mode = OutputSettingsMode::NETWORK;

            // Check if next arguments look like IP and port
            // If these start with -, it means that these weren't provided
            if (arg_index + 1 < argc && argv[arg_index + 1][0] != '-') {
                // First the IP
                std::strcpy(settings.output_settings.network_data.ip, argv[++arg_index]);

                // Check for optional port
                if (arg_index + 1 < argc && argv[arg_index + 1][0] != '-') {
                    settings.output_settings.network_data.port = std::atoi(argv[++arg_index]);
                }

                // Check for optional uuid
                if (arg_index + 1 < argc && argv[arg_index + 1][0] != '-') {
                    std::strcpy(
                        settings.output_settings.network_data.uuid,
                        argv[++arg_index]);
                }
            }
            continue;
        }

        if (!strcmp(parameter, "--output_disabled")) {
            settings.output_settings.mode = OutputSettingsMode::DISABLED;
            continue;
        }

        // Arguments with a single parameter ---------------------------------------------------
        // Make sure there's a value for the parameter
        if (arg_index + 1 >= argc) {
            LOG_WARNING("Missing value of parameter \"" << parameter << "\"");
            break;
        }

        const char* value = argv[++arg_index];

        if (!strcmp(parameter, "-w") || !strcmp(parameter, "--width")) {
            settings.image.width = std::atoi(value);
        } else if (!strcmp(parameter, "-h") || !strcmp(parameter, "--height")) {
            settings.image.height = std::atoi(value);
        } else if (!strcmp(parameter, "-s") || !strcmp(parameter, "--samples")) {
            settings.image.multi_sample_anti_aliasing = std::atoi(value);
            uint32_t n_samples = std::sqrt(settings.image.multi_sample_anti_aliasing);

            if (n_samples * n_samples != settings.image.multi_sample_anti_aliasing) {
                LOG_WARNING("Sample count must be a perfect square");
                settings.image.multi_sample_anti_aliasing = 1;
            }

        } else if (!strcmp(parameter, "-b") || !strcmp(parameter, "--block_size")) {
            settings.block_size = std::atoi(value);
        } else if (!strcmp(parameter, "-z") || !strcmp(parameter, "--zoom")) {
            DESERIALIZE_NUM(settings.camera.zoom, value);
        } else if (!strcmp(parameter, "-cx") || !strcmp(parameter, "--camera_x")) {
            DESERIALIZE_NUM(settings.camera.x, value);
        } else if (!strcmp(parameter, "-cy") || !strcmp(parameter, "--camera_y")) {
            DESERIALIZE_NUM(settings.camera.y, value);
        } else if (!strcmp(parameter, "-i") || !strcmp(parameter, "--iterations")) {
            settings.fractal.max_iterations = std::atoi(value);
        } else if (!strcmp(parameter, "-t") || !strcmp(parameter, "--type")) {
            uint32_t fractal_type = std::atoi(value);
            if (fractal_type >= (uint32_t)FractalType::INVALID_LAST) {
                LOG_WARNING("Trying to assign an invalid color mode value");
            } else {
                settings.fractal.type = static_cast<FractalType>(fractal_type);
            }
        } else if (!strcmp(parameter, "--color_mode")) {
            uint32_t color_mode = std::atoi(value);
            if (color_mode >= (uint32_t)ColorMode::INVALID_LAST) {
                LOG_WARNING("Trying to assign an invalid color mode value");
            } else {
                settings.fractal.color_mode = static_cast<ColorMode>(color_mode);
            }
        } else if (!strcmp(parameter, "--julia-cx")) {
            settings.fractal.julia_settings.Cx = atof(value);
        } else if (!strcmp(parameter, "--julia-cy")) {
            settings.fractal.julia_settings.Cy = atof(value);
        } else {
            LOG_WARNING("Unrecognized parameter \"" << parameter << "\"");
        }
    }
    return true;
}
