#pragma once

enum class OutputSettingsMode {
    DISK,
    NETWORK,
    DISABLED
};

struct OutputSetingsDiskData {

    /// @brief Path where the image gets saved
    char output_path[1024];

    OutputSetingsDiskData()
        : output_path("./output.png")
    {
    }
};

struct OutputSettingsNetworkData {
    char ip[128];
    int port;
    char uuid[37];

    OutputSettingsNetworkData()
        : ip("0.0.0.0")
        , port(5001)
        , uuid("00000000-0000-0000-0000-000000000000")
    {
    }
};

struct OutputSettings {

    OutputSettingsMode mode;

    OutputSetingsDiskData disk_data;
    OutputSettingsNetworkData network_data;

    OutputSettings()
        : mode(OutputSettingsMode::DISK)
    {
    }
};
