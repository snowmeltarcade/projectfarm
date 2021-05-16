#include <array>

#include "utils/util.h"
#include "client_server_world_loaded.h"

namespace projectfarm::shared::networking::packets
{
    void ClientServerWorldLoaded::SerializeBytes(std::vector<std::byte>&) const noexcept
    {
    }

    void ClientServerWorldLoaded::FromBytes(const std::vector<std::byte>&)
    {
    }

    void ClientServerWorldLoaded::OutputDebugData(std::stringstream&) const noexcept
    {
    }
}