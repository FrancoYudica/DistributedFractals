#pragma once
#include "settings/settings.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

void print_help()
{
    std::cout << "Fractal Renderer - Command Line Options\n";
    std::cout << "----------------------------------------\n";
    std::cout << "  -o,  --output       <path>     Output file path\n";
    std::cout << "  -w,  --width        <int>      Image width in pixels\n";
    std::cout << "  -h,  --height       <int>      Image height in pixels\n";
    std::cout << "  -s,  --samples      <int>      Number of MSAA samples\n";
    std::cout << "  -b,  --block_size   <int>      Size in pixels of the MPI image task\n";
    std::cout << "  -z,  --zoom         <float>    Zoom level of camera\n";
    std::cout << "  -cx, --camera_x     <float>    Camera X position\n";
    std::cout << "  -cy, --camera_y     <float>    Camera Y position\n";
    std::cout << "  -i,  --iterations   <int>      Max iterations for fractal\n";
    std::cout << "  -t,  --type         <int>      Fractal type ID\n";
    std::cout << "  --color_mode        <int>      Color mode type ID\n";
    std::cout << "  --julia-cx          <float>    Real component of Julia set C constant\n";
    std::cout << "  --julia-cy          <float>    Imaginary component of Julia set C constant\n";
    std::cout << "  --help                         Show this help message\n";
}

void load_args(int argc, char** argv, Settings& settings)
{
    for (int arg_index = 1; arg_index < argc; ++arg_index) {
        const char* parameter = argv[arg_index];

        // Help command
        if (!strcmp(parameter, "--help")) {
            print_help();
            std::exit(0); // Exit after printing help
        }

        // Make sure there's a value for the parameter
        if (arg_index + 1 >= argc) {
            std::cout << "Missing value of parameter \"" << parameter << "\"\n";
            break;
        }

        const char* value = argv[++arg_index];

        if (!strcmp(parameter, "-o") || !strcmp(parameter, "--output")) {
            std::strcpy(settings.output_path, value);
        } else if (!strcmp(parameter, "-w") || !strcmp(parameter, "--width")) {
            settings.image.width = std::atoi(value);
        } else if (!strcmp(parameter, "-h") || !strcmp(parameter, "--height")) {
            settings.image.height = std::atoi(value);
        } else if (!strcmp(parameter, "-s") || !strcmp(parameter, "--samples")) {
            settings.image.multi_sample_anti_aliasing = std::atoi(value);
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
