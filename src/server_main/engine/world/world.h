#ifndef PROJECTFARM_WORLD_H
#define PROJECTFARM_WORLD_H

#include <string>
#include <filesystem>
#include <thread>
#include <vector>
#include <list>
#include <memory>
#include <map>
#include <tuple>
#include <nlohmann/json.hpp>

#include "data/consume_data_provider.h"
#include "engine/entities/entity.h"
#include "time/timer.h"
#include "server/consume_server.h"
#include "island.h"
#include "plots.h"
#include "engine/player.h"
#include "networking/packet.h"
#include "networking/consume_packet_sender.h"
#include "engine/entities/character.h"
#include "engine/entities/consume_action_animations_manager.h"
#include "time/consume_timer.h"
#include "scripting/script.h"
#include "scripting/script_system.h"
#include "scripting/consume_script_system.h"
#include "engine/world/action_tile_actions/action_tile_action_base.h"
#include "engine/data/consume_data_manager.h"

namespace projectfarm::engine::world
{
    class World final : public shared::ConsumeDataProvider,
                        public server::ConsumeServer,
                        public shared::networking::ConsumePacketSender,
                        public shared::scripting::ConsumeScriptSystem,
                        public shared::time::ConsumeTimer,
                        public engine::entities::ConsumeActionAnimationsManager,
                        public engine::data::ConsumeDataManager,
                        public std::enable_shared_from_this<World>
    {
    public:
        World()
        {
            this->_plots = std::make_shared<Plots>();
        }
        ~World() override = default;

        [[nodiscard]] bool Load(const std::string& name, const std::filesystem::path& filePath);
        void Shutdown();

        [[nodiscard]] const std::string& GetName() const noexcept
        {
            return this->_name;
        }

        [[nodiscard]] std::vector<std::byte> GetDataForClientSerialization() const noexcept;

        void Tick();

        [[nodiscard]] bool AddPlayer(const std::shared_ptr<engine::Player>& player, uint32_t entityId,
                                     bool shouldUseSpawnPoints = true, bool isNewPlayerToGame = false) noexcept;

        void RemovePlayer(uint32_t playerId) noexcept;

        [[nodiscard]] bool TransferPlayerToWorld(uint32_t playerId, uint32_t entityId,
                                                 const std::string& destinationWorldName) noexcept;

        [[nodiscard]] bool TransferCharacterToWorld(const std::shared_ptr<entities::Character>& character,
                                                    const std::string& destinationWorldName,
                                                    const std::string& destinationTileType) noexcept;

        void OnReceivePacket(const std::shared_ptr<shared::networking::Packet>& packet,
                             const std::shared_ptr<engine::Player>& player) noexcept;

        [[nodiscard]] std::vector<std::shared_ptr<entities::Character>> GetCharactersWithinDistance(
                uint32_t characterUniqueId, float distance) const noexcept;

        [[nodiscard]] uint16_t GetPlotIndexFromWorldPosition(float x, float y) const noexcept;
        [[nodiscard]] std::tuple<uint8_t, int32_t, int32_t> GetTileIndexesFromWorldPosition(float x, float y) const noexcept;

        [[nodiscard]] const std::shared_ptr<Plots>& GetPlots() const noexcept
        {
            return this->_plots;
        }

        [[nodiscard]] std::map<uint8_t, std::vector<ActionTile>>
            GetActionTilesByProperty(const std::string& name, const std::string& value) const noexcept;

        void TriggerActionTiles(float worldX, float worldY,
                                const std::shared_ptr<entities::Character>& character) noexcept;

        [[nodiscard]] std::shared_ptr<entities::Character>
                AddCharacter(uint32_t tileX, uint32_t tileY, uint8_t islandId,
                             const std::string& type, uint32_t entityId, uint32_t playerId) noexcept;

        [[nodiscard]] std::shared_ptr<entities::Character>
            AddCharacter(uint32_t xPos, uint32_t yPos, const std::string& type,
                     uint32_t entityId, uint32_t playerId) noexcept;

        [[nodiscard]] std::shared_ptr<entities::Character>
                AddCharacter(const std::string& characterType,
                             const std::string& destinationTileType,
                             uint32_t entityId, uint32_t playerId) noexcept;

        [[nodiscard]] std::vector<std::shared_ptr<Island>>& GetIslands() noexcept
        {
            return this->_islands;
        }

    private:
        std::string _name;
        std::vector<std::shared_ptr<Island>> _islands;

        [[nodiscard]] bool LoadFromJsonFile(const std::filesystem::path& filePath);
        [[nodiscard]] bool LoadFromBinaryFile(const std::filesystem::path& filePath);

        [[nodiscard]] bool LoadFromFile(const std::filesystem::path& filePath) noexcept;
        [[nodiscard]] bool LoadIsland(const nlohmann::json& islandJson); // can throw
        [[nodiscard]] bool LoadIsland(std::ifstream& fs) noexcept;

        [[nodiscard]] bool InitiateScript() noexcept;

        [[nodiscard]] bool AddWorldEntity() noexcept;
        [[nodiscard]] bool RemoveWorldEntity(uint32_t entityId) noexcept;
        [[nodiscard]] bool RemoveWorldEntity(const std::shared_ptr<engine::entities::Entity>& entity) noexcept;

        template <typename T, typename... Args>
        [[nodiscard]] std::shared_ptr<T> CreateEntity(uint32_t entityId, Args... args) const noexcept;

        void UpdateEntities();

        void BroadcastEntityState(const std::shared_ptr<engine::entities::Entity>& entity, uint64_t currentTime) const noexcept;
        void SendPacketToAllPlayers(const std::shared_ptr<shared::networking::Packet>& packet,
                                    uint32_t exceptPlayerId = 0, uint64_t milliseconds = 0,
                                    bool forcePacketVital = false) const noexcept;

        std::list<std::shared_ptr<entities::Entity>> _entities;
        std::list<uint32_t> _players;

        std::shared_ptr<shared::time::Timer> _timer;

        std::shared_ptr<Plots> _plots;

        std::shared_ptr<shared::scripting::Script> _script;

        [[nodiscard]]
        std::shared_ptr<entities::Character> CreateCharacter(const std::string& type,
                                                             uint32_t entityId, uint32_t playerId = 0) noexcept;

        void HandleClientServerWorldLoadedPacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                                 const std::shared_ptr<engine::Player>& player) noexcept;

        void HandleClientServerEntityUpdatePacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                                  const std::shared_ptr<engine::Player>& player) noexcept;

        void HandleClientServerChatboxMessagePacket(const std::shared_ptr<shared::networking::Packet>& packet,
                                                    const std::shared_ptr<engine::Player>& player) noexcept;

        std::vector<std::shared_ptr<world::action_tile_actions::ActionTileActionBase>> _actionTileActions;

        void PushActionTileAction(const std::shared_ptr<world::action_tile_actions::ActionTileActionBase>& action) noexcept
        {
            this->_actionTileActions.push_back(action);
        }

        void ProcessActionTileActions() noexcept;

        void SendSetCharacterDetailsPacket(const std::shared_ptr<entities::Entity>& entity) noexcept;

        void BroadcastChatboxMessage(const std::string& message,
                                     const std::string& fromUsername,
                                     uint32_t exceptPlayerId = 0) noexcept;

        void BroadcastChatboxSystemMessage(const std::string& message, uint32_t exceptPlayerId = 0) noexcept
        {
            this->BroadcastChatboxMessage(message, "System", exceptPlayerId);
        }
    };
}

#endif
