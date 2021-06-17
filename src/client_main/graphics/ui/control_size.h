#ifndef PROJECTFARM_CONTROL_SIZE_H
#define PROJECTFARM_CONTROL_SIZE_H

#include <cstdint>

namespace projectfarm::graphics::ui
{
    class ControlSize final
    {
    public:
        ControlSize() = default;
        ~ControlSize() = default;

        void SetParentSize(const ControlSize& parentSize) noexcept
        {
            this->SetParentSize(parentSize.GetWidth(), parentSize.GetHeight());
        }

        void SetParentSize(uint32_t parentWidth, uint32_t parentHeight) noexcept
        {
            this->_parentWidth = parentWidth;
            this->_parentHeight = parentHeight;
        }

        [[nodiscard]]
        uint32_t GetWidth() const noexcept
        {
            return this->_width;
        }

        [[nodiscard]]
        uint32_t GetHeight() const noexcept
        {
            return this->_height;
        }

        // this is useful if the parent size gets updated for instance
        void ReconfirmSize() noexcept
        {
            this->SetSizePercent(this->_widthPercent, this->_heightPercent);
        }

        void SetSize(uint32_t width, uint32_t height) noexcept
        {
            this->_width = width;
            this->_height = height;

            this->_widthPercent = ((float)this->_parentWidth / 100.0f) * (float)this->_width;
            this->_heightPercent = ((float)this->_parentHeight / 100.0f) * (float)this->_height;
        }

        [[nodiscard]]
        float GetHeightPercent() const noexcept
        {
            return this->_heightPercent;
        }

        void SetSizePercent(float widthPercent, float heightPercent) noexcept
        {
            this->_widthPercent = widthPercent;
            this->_heightPercent = heightPercent;

            this->_width = static_cast<uint32_t>(this->_parentWidth * (widthPercent / 100.0f));
            this->_height = static_cast<uint32_t>(this->_parentHeight * (heightPercent / 100.0f));
        }

        [[nodiscard]]
        ControlSize CreateChildSize() const noexcept
        {
            ControlSize size;
            size.SetParentSize(this->_width, this->_height);
            return size;
        }

    private:
        uint32_t _parentWidth {0};
        uint32_t _parentHeight {0};

        uint32_t _width  {0};
        uint32_t _height {0};

        float _widthPercent {0.0f};
        float _heightPercent {0.0f};
    };
}

#endif
