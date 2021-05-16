#ifndef PROJECTFARM_DATA_PROVIDER_LOCATIONS_H
#define PROJECTFARM_DATA_PROVIDER_LOCATIONS_H

namespace projectfarm::shared
{
    enum class DataProviderLocations : uint8_t
    {
        Client,
        ClientUI,
        ClientUICustomControls,
        ClientFonts,
        ClientTileSets,
        ClientTileMaps,
        ClientGraphicsMaterials,
        ClientGraphicsShaders,
        Server,
        ServerCharacters,
        ServerScriptsCharacters,
        ServerScriptsWorlds,
        ServerDatabases,
        SharedWorlds,
        SharedCharacters,
        SharedScripting,
    };
}

#endif
