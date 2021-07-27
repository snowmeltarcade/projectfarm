#ifndef PROJECTFARM_LOCATIONS_H
#define PROJECTFARM_LOCATIONS_H

namespace projectfarm::shared::api::data
{
    enum class Locations : uint8_t
    {
        Client,
        ClientUI,
        ClientUICustomControls,
        ClientUICSSDefault,
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
