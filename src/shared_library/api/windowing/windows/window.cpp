#include <string>

#include "window.h"
#include "api/logging/logging.h"
#include "game/concurrency_keys.h"
#include "concurrency/state.h"

using namespace std::literals;

namespace projectfarm::shared::api::windowing::windows
{
    bool Window::LoadFromConfig(const std::filesystem::path& configPath) noexcept
    {
        auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                     SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN;

        this->_window = SDL_CreateWindow("title",
                                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                         500, 500,
                                         flags);
        if (!this->_window)
        {
            api::logging::Log("Failed to create window: "s + SDL_GetError());
            return false;
        }

        this->_context = SDL_GL_CreateContext(this->_window);
        if (!this->_context)
        {
            api::logging::Log("Failed to create GL context: "s + SDL_GetError());
            return false;
        }

        return true;
    }

    void Window::PumpMessages()
    {
        SDL_Event e;

        while (SDL_PollEvent(&e))
        {
            //engine::UpdateDeviceCapabilities(e);

            switch (e.type)
            {
                case SDL_WINDOWEVENT:
                {
                    switch (e.window.event)
                    {
//                        case SDL_WINDOWEVENT_RESIZED:
//                        {
//                            this->GetGraphics()->OnWindowResized(static_cast<uint32_t>(e.window.data1),
//                                                                 static_cast<uint32_t>(e.window.data2));
//                            break;
//                        }
                        case SDL_WINDOWEVENT_CLOSE:
                        {
                            shared::api::logging::Log("Event: Quit");

                            concurrency::state::SetBool(game::ConcurrencyKeyRunning, false);
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    break;
                }
//                case SDL_KEYDOWN:
//                case SDL_KEYUP:
//                {
//                    this->_keyboardInput->UpdateKeyboardKey(e.key);
//                    break;
//                }
                case SDL_APP_TERMINATING:
                {
                    shared::api::logging::Log("Event: Terminating");

                    concurrency::state::SetBool(game::ConcurrencyKeyRunning, false);
                    break;
                }
//                case SDL_APP_LOWMEMORY:
//                {
//                    shared::api::logging::Log("Event: Low Memory");
//
//                    this->LowerMemoryPressure();
//                    break;
//                }
//                case SDL_APP_WILLENTERBACKGROUND:
//                {
//                    this->EnterLowerActivityState();
//
//                    shared::api::logging::Log("Event: Will Enter Background");
//                    break;
//                }
//                case SDL_APP_DIDENTERBACKGROUND:
//                {
//                    shared::api::logging::Log("Event: Did Enter Background");
//                    break;
//                }
//                case SDL_APP_WILLENTERFOREGROUND:
//                {
//                    this->ResumeFromLowerActivityState();
//
//                    shared::api::logging::Log("Event: Will Enter Foreground");
//                    break;
//                }
//                case SDL_APP_DIDENTERFOREGROUND:
//                {
//                    shared::api::logging::Log("Event: Did Enter Foreground");
//                    break;
//                }
                case SDL_QUIT:
                {
                    shared::api::logging::Log("Event: Quit");

                    concurrency::state::SetBool(game::ConcurrencyKeyRunning, false);
                    break;
                }
//                case SDL_MOUSEMOTION:
//                {
//                    if (e.motion.which == SDL_TOUCH_MOUSEID)
//                    {
//                        break;
//                    }
//
//                    this->_mouseInput->SetPosition(static_cast<uint32_t>(e.motion.x),
//                                                   static_cast<uint32_t>(e.motion.y));
//
//                    break;
//                }
//                case SDL_FINGERMOTION:
//                {
//                    auto [x, y] = this->GetGraphics()->GetCamera()->GetPositionAtPercent(e.tfinger.x, e.tfinger.y);
//
//                    this->_mouseInput->SetPosition(x, y);
//
//                    break;
//                }
//                case SDL_MOUSEBUTTONDOWN:
//                {
//                    if (e.button.which == SDL_TOUCH_MOUSEID)
//                    {
//                        break;
//                    }
//
//                    if (e.button.button == SDL_BUTTON_LEFT)
//                    {
//                        this->_mouseInput->SetButtonState(true);
//                    }
//
//                    break;
//                }
//                case SDL_FINGERDOWN:
//                {
//                    auto [x, y] = this->GetGraphics()->GetCamera()->GetPositionAtPercent(e.tfinger.x, e.tfinger.y);
//
//                    this->_mouseInput->SetPosition(x, y);
//
//                    this->_mouseInput->SetButtonState(true);
//
//                    break;
//                }
//                case SDL_MOUSEBUTTONUP:
//                {
//                    if (e.button.which == SDL_TOUCH_MOUSEID)
//                    {
//                        break;
//                    }
//
//                    if (e.button.button == SDL_BUTTON_LEFT)
//                    {
//                        this->_mouseInput->SetButtonState(false);
//                    }
//
//                    break;
//                }
//                case SDL_FINGERUP:
//                {
//                    auto [x, y] = this->GetGraphics()->GetCamera()->GetPositionAtPercent(e.tfinger.x, e.tfinger.y);
//
//                    this->_mouseInput->SetPosition(x, y);
//
//                    this->_mouseInput->SetButtonState(false);
//
//                    break;
//                }
                default:
                {
                    break;
                }
            }
        }
    }

    bool Window::SetTitle(std::string_view title)
    {
        if (!this->_window)
        {
            logging::Log("Window is null - cannot set title.");
            return false;
        }

        // ensure the title is null terminated
        SDL_SetWindowTitle(this->_window, std::string(title).c_str());

        return true;
    }

    void Window::Shutdown() noexcept
    {
        if (this->_context)
        {
            SDL_GL_DeleteContext(this->_context);
            this->_context = nullptr;
        }

        if (this->_window)
        {
            SDL_DestroyWindow(this->_window);
            this->_window = nullptr;
        }
    }
}
