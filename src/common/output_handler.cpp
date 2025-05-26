#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include "common/output_handler.h"
#include "image_utils.h"
#include "common/common.h"
#include "common/logging.h"

std::shared_ptr<OutputHandler> OutputHandler::factory_create(OutputSettingsMode mode)
{
    switch (mode) {
    case OutputSettingsMode::DISK:
        return std::make_shared<DiskOutputHandler>();

    case OutputSettingsMode::NETWORK:
        return std::make_shared<NetworkOutputHandler>();

    case OutputSettingsMode::DISABLED:
        return std::make_shared<OutputHandler>();

    default:
        return nullptr;
    }
}

bool DiskOutputHandler::save_output(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    const OutputSettings& settings)
{
    return save_image(
        settings.disk_data.output_path,
        image.data(),
        width,
        height);
}

bool NetworkOutputHandler::save_output(
    const std::vector<uint8_t>& image,
    int width,
    int height,
    const OutputSettings& settings)
{

    std::vector<uint8_t> png_buffer;
    bool success = save_image_to_memory(
        png_buffer,
        image.data(),
        width,
        height);

    if (!success) {
        LOG_ERROR("Unable to create png buffer");
        return false;
    }

    LOG_STATUS("PNG buffer created successfully. Sending buffer to server");

    // creating socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        LOG_ERROR("Socket creation failed");
        return false;
    }

    // specifying address
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(settings.network_data.port);

    if (inet_pton(AF_INET, settings.network_data.ip, &serverAddress.sin_addr) <= 0) {
        LOG_ERROR("Invalid IP address");
        close(clientSocket);
        return false;
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        LOG_ERROR("Connection failed");
        close(clientSocket);
        return false;
    }

    // Sends UUID size and UUID
    uint32_t uuid_len = static_cast<uint32_t>(std::strlen(settings.network_data.uuid));
    uint32_t net_uuid_len = htonl(uuid_len);
    send(clientSocket, &net_uuid_len, sizeof(net_uuid_len), 0);
    send(clientSocket, settings.network_data.uuid, uuid_len, 0);

    // Sends buffer size
    uint32_t image_byte_size = png_buffer.size();
    uint32_t net_image_byte_size = htonl(image_byte_size);
    send(clientSocket, &net_image_byte_size, sizeof(net_image_byte_size), 0);

    // Sends buffer
    send(clientSocket, png_buffer.data(), png_buffer.size(), 0);

    // closing socket
    close(clientSocket);
    return true;
}
