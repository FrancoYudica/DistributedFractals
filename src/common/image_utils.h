#pragma once
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <png.h>

bool save_image(
    const char* filename,
    const uint8_t* data,
    int width,
    int height)
{
    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        perror("Error while trying to open file in write mode");
        return false;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        fprintf(stderr, "Unable to create PNG struct\n");
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, nullptr);
        fprintf(stderr, "Unable to create PNG info_struct\n");
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fprintf(stderr, "Fail during PNG write\n");
        return false;
    }

    png_init_io(png_ptr, fp);

    // Header specification
    png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    // Writes all the rows
    for (int y = 0; y < height; y++) {
        png_bytep row = (png_bytep)&data[y * width * 3];
        png_write_row(png_ptr, row);
    }
    png_write_end(png_ptr, nullptr);

    fclose(fp);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return true;
}

// Custom write callback: appends bytes to a std::vector
void _png_memory_write(png_structp png_ptr, png_bytep data, png_size_t length)
{
    auto* out = reinterpret_cast<std::vector<uint8_t>*>(png_get_io_ptr(png_ptr));
    out->insert(out->end(), data, data + length);
}

// Optional: flush callback (does nothing for memory)
void _png_memory_flush(png_structp)
{
}

bool save_image_to_memory(
    std::vector<uint8_t>& out_buffer,
    const uint8_t* data,
    int width,
    int height)
{
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fprintf(stderr, "Unable to create PNG struct\n");
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fprintf(stderr, "Unable to create PNG info_struct\n");
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fprintf(stderr, "Fail during PNG write\n");
        return false;
    }

    // Use our custom write function
    png_set_write_fn(png_ptr, &out_buffer, _png_memory_write, _png_memory_flush);

    png_set_IHDR(png_ptr, info_ptr, width, height,
        8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    for (int y = 0; y < height; y++) {
        png_bytep row = (png_bytep)&data[y * width * 3];
        png_write_row(png_ptr, row);
    }

    png_write_end(png_ptr, nullptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return true;
}
