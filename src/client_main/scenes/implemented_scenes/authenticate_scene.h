#ifndef AUTHENTICATE_SCENE_H
#define AUTHENTICATE_SCENE_H

#include <memory>

#include "../scene.h"
#include "graphics/ui/ui.h"
#include "graphics/ui/label.h"
#include "graphics/ui/mouse_cursor.h"
#include "graphics/shader_program.h"
#include "time/stopwatch.h"

namespace projectfarm::scenes::implemented_scenes
{
    class AuthenticateScene final : public Scene
    {
    public:
        static inline const std::string LoadParameter_UserName { "username" };
        static inline const std::string LoadParameter_Password { "password" };

        AuthenticateScene() = default;
        ~AuthenticateScene() override = default;

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

        void OnPlayerCharacterAdd(const std::shared_ptr<engine::entities::Character>&) noexcept override
        {
        }

        void ReconfirmPixelSizes() noexcept override
        {
            this->_ui->ReconfirmSize();
        }

        [[nodiscard]]
        std::string SendUIMessage(const std::string& key,
                                  const std::vector<projectfarm::shared::scripting::FunctionParameter>& parameters) noexcept override;

    private:
        bool _shouldQuit {false};

        std::string _userName;
        std::string _password;

        bool _shouldLogIn {false};
        bool _loggingIn {false};

        bool _performUdpTest {false};
        uint64_t _udpTestCounter {0};

        shared::time::Stopwatch _loginStopwatch;
        uint8_t _loginStopwatchCounter {0u};

        bool _isLoadingNewScene {false};

        std::shared_ptr<projectfarm::graphics::ui::UI> _ui;
        std::shared_ptr<projectfarm::graphics::ui::Label> _labelStatus;
        std::shared_ptr<projectfarm::graphics::ui::MouseCursor> _mouseCursor;

        [[nodiscard]] bool ValidatePacket(const std::shared_ptr<shared::networking::Packet>& packet) const;

        void HandleServerClientSetPlayerDetailsPacket(const std::shared_ptr<shared::networking::Packet>& packet);
        void HandleServerClientLoadWorldPacket(const std::shared_ptr<shared::networking::Packet>& packet);
        void HandleServerClientSendHashedPasswordPacket(const std::shared_ptr<shared::networking::Packet>& packet);

        [[nodiscard]] bool SetupUI();

        void LoadNewWorld(const std::string& worldName) noexcept;

        [[nodiscard]] bool StartLoggingIn() noexcept;
        [[nodiscard]] bool SetupLoggingInUI() noexcept;

        void SendServerAuthenticatePacket(const std::string& hashedPassword) noexcept;
        void SendServerRequestHashedPasswordPacket() noexcept;

        void PerformUdpTest() noexcept;

        void SetSceneStatus(const std::string& status) const noexcept
        {
            this->_ui->SetSimpleBinding("loading_status", status);
        }
    };
}

#endif
