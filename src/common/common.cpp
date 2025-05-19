#include "common.h"

void print_help()
{
    std::cout << "Fractal Renderer - Command Line Options\n";
    std::cout << "----------------------------------------\n";
    std::cout << "  -od, --output_disk       [opt filename]         Save output image to disk. Defaults to 'output.png' if no filename is provided.\n";
    std::cout << "  -on, --output_network    [opt IP [opt port]]    Send output image over TCP. Defaults to IP 0.0.0.0 and port 5001 if not specified.\n";
    std::cout << "  -w,  --width             <int>                  Image width in pixels\n";
    std::cout << "  -h,  --height            <int>                  Image height in pixels\n";
    std::cout << "  -s,  --samples           <int>                  Number of MSAA samples\n";
    std::cout << "  -b,  --block_size        <int>                  Size in pixels of the MPI image task\n";
    std::cout << "  -z,  --zoom              <float>                Zoom level of camera\n";
    std::cout << "  -cx, --camera_x          <float>                Camera X position\n";
    std::cout << "  -cy, --camera_y          <float>                Camera Y position\n";
    std::cout << "  -i,  --iterations        <int>                  Max iterations for fractal\n";
    std::cout << "  -t,  --type              <int>                  Fractal type ID\n";
    std::cout << "  --color_mode             <int>                  Color mode type ID\n";
    std::cout << "  --julia-cx               <float>                Real component of Julia set C constant\n";
    std::cout << "  --julia-cy               <float>                Imaginary component of Julia set C constant\n";
    std::cout << "  --help                                          Show this help message\n";
}

void load_args(int argc, char** argv, Settings& settings)
{
    for (int arg_index = 1; arg_index < argc; ++arg_index) {
        const char* parameter = argv[arg_index];

        // Help command -----------------------------------------------------------------------
        if (!strcmp(parameter, "--help")) {
            print_help();
            std::exit(0); // Exit after printing help
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
            std::cout << "Missing value of parameter \"" << parameter << "\"\n";
            break;
        }

        const char* value = argv[++arg_index];

        if (!strcmp(parameter, "-w") || !strcmp(parameter, "--width")) {
            settings.image.width = std::atoi(value);
        } else if (!strcmp(parameter, "-h") || !strcmp(parameter, "--height")) {
            settings.image.height = std::atoi(value);
        } else if (!strcmp(parameter, "-s") || !strcmp(parameter, "--samples")) {
            settings.image.multi_sample_anti_aliasing = std::atoi(value);
            int n_samples = std::sqrt(settings.image.multi_sample_anti_aliasing);

            if (n_samples * n_samples != settings.image.multi_sample_anti_aliasing) {
                std::cout << "Sample count must be a perfect square" << std::endl;
                settings.image.multi_sample_anti_aliasing = 1;
            }

        } else if (!strcmp(parameter, "-b") || !strcmp(parameter, "--block_size")) {
            settings.block_size = std::atoi(value);
        } else if (!strcmp(parameter, "-z") || !strcmp(parameter, "--zoom")) {
            settings.camera.zoom = std::atof(value);
        } else if (!strcmp(parameter, "-cx") || !strcmp(parameter, "--camera_x")) {
            settings.camera.x = std::atof(value);
        } else if (!strcmp(parameter, "-cy") || !strcmp(parameter, "--camera_y")) {
            settings.camera.y = std::atof(value);
        } else if (!strcmp(parameter, "-i") || !strcmp(parameter, "--iterations")) {
            settings.fractal.max_iterations = std::atoi(value);
        } else if (!strcmp(parameter, "-t") || !strcmp(parameter, "--type")) {
            int fractal_type = std::atoi(value);
            if (fractal_type >= (int)FractalType::INVALID_LAST) {
                std::cout << "Trying to assign an invalid color mode value" << std::endl;
            } else {
                settings.fractal.type = static_cast<FractalType>(fractal_type);
            }
        } else if (!strcmp(parameter, "--color_mode")) {
            int color_mode = std::atoi(value);
            if (color_mode >= (int)ColorMode::INVALID_LAST) {
                std::cout << "Trying to assign an invalid color mode value" << std::endl;
            } else {
                settings.fractal.color_mode = static_cast<ColorMode>(color_mode);
            }
        } else if (!strcmp(parameter, "--julia-cx")) {
            settings.fractal.julia_settings.Cx = atof(value);
        } else if (!strcmp(parameter, "--julia-cy")) {
            settings.fractal.julia_settings.Cy = atof(value);
        } else {
            std::cout << "Unrecognized parameter \"" << parameter << "\"\n";
        }
    }
}
