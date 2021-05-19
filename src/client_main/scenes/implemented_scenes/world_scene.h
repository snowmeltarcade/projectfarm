#ifndef PROJECTFARM_WORLD_SCENE_H
#define PROJECTFARM_WORLD_SCENE_H

#include <filesystem>
#include <unordered_map>

#include "scenes/scene.h"
#include "graphics/ui/ui.h"
#include "graphics/ui/label.h"
#include "engine/world/world.h"
#include "entities/character_appearance_details.h"

namespace projectfarm::scenes::implemented_scenes
{
    class WorldScene final : public Scene
    {
    public:
        static inline const std::string LoadParameter_WorldName = "worldname";

        WorldScene() = default;
        ~WorldScene() override = default;

        [[nodiscard]]
        bool ShouldQuit() const override
        {
            return this->_shouldQuit;
        }

        void ProcessPacket(const std::shared_ptr<shared::networking::Packet>& packet) override;

        void UpdateInput() override;

        bool Initialize() override;

        void PrepareRender() override;

        void Tick() override;

        void Destroy() override;

        void Quit() override;

        void OnPlayerCharacterAdd(const std::shared_ptr<engine::entities::Character>& character) noexcept override;

        void ReconfirmPixelSizes() noexcept override;

        [[nodiscard]]
        std::string SendUIMessage(const std::string& key,
                                  const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept override;

    private:
        bool _shouldQuit {false};

        std::shared_ptr<engine::world::World> _world;

        std::shared_ptr<projectfarm::graphics::ui::UI> _ui;
        bool _isUIInFocus {false};

        std::unordered_map<uint32_t, shared::entities::CharacterAppearanceDetails> _cachedCharacterDetails;

        bool _computeDebugInfo {false};

        [[nodiscard]]
        bool ValidatePacket(const std::shared_ptr<shared::networking::Packet>& packet) const;

        void HandleServerClientLoadWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet) const;
        void HandleServerClientEntityUpdatePacket(const std::shared_ptr<shared::networking::Packet>& packet);
        void HandleServerClientPlayerJoinedWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet) const;
        void HandleServerClientPlayerLeftWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet) const;
        void HandleServerClientRemoveEntityFromWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet);
        void HandleServerClientCharacterSetDetailsPacket(const std::shared_ptr<shared::networking::Packet>& packet);
        void HandleServerClientChatboxMessagePacket(const std::shared_ptr<shared::networking::Packet>& packet);

        [[nodiscard]]
        bool SetupUI();

        [[nodiscard]]
        bool LoadWorldFile(const std::string& worldName);

        void TellServerWorldHasBeenLoaded();

        void UpdateUI() noexcept;

        void UpdateDebugInfo();

        [[nodiscard]]
        std::string ProcessChatboxCommand(const std::string& command,
                                          const std::vector<std::string>& parameters) noexcept;
    };
}

#endif
