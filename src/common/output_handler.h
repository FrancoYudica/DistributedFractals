#pragma once
#include <vector>
#include <memory>
#include <stdint.h>
#include "common/settings/output_settings.h"

// Forward declarations
class DiskOutputHandler;
class NetworkOutputHandler;

class OutputHandler {

public:
    OutputHandler() = default;

    /// @brief Persists image
    virtual bool save_output(
        const std::vector<uint8_t>& image,
        int width,
        int height,
        const OutputSettings& settings)
    {
        return true;
    };

    /// @brief Given the mode, returns the OutputHandler class
    static std::shared_ptr<OutputHandler> factory_create(OutputSettingsMode mode);
};

/// @brief Stores image into disk
class DiskOutputHandler : public OutputHandler {

public:
    DiskOutputHandler() = default;

    bool save_output(
        const std::vector<uint8_t>& image,
        int width,
        int height,
        const OutputSettings& settings);
};

/// @brief Sends image through network
class NetworkOutputHandler : public OutputHandler {

public:
    NetworkOutputHandler() = default;

    bool save_output(
        const std::vector<uint8_t>& image,
        int width,
        int height,
        const OutputSettings& settings);
};
