#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <filesystem>

#include "logging/consume_logger.h"
#include "graphics/consume_graphics.h"
#include "graphics/render_origin_points.h"

namespace projectfarm::graphics
{
	class Renderable : public shared::ConsumeLogger,
					   public ConsumeGraphics
	{
	public:
		Renderable() = default;
		~Renderable() override = default;

		virtual void Render() noexcept = 0;

        void SetRenderDetails(int32_t x, int32_t y) noexcept
        {
            this->_renderX = x;
            this->_renderY = y;
        }

		void SetRenderDetails(int32_t x, int32_t y, uint32_t width, uint32_t height, RenderOriginPoints renderOriginPoint) noexcept
        {
		    this->_renderX = x;
		    this->_renderY = y;
		    this->_width = width;
		    this->_height = height;
		    this->_renderOriginPoint = renderOriginPoint;
        }

        void SetOriginDetails(uint32_t originX, uint32_t originY, uint32_t originWidth, uint32_t originHeight) noexcept
        {
		    this->_originX = originX;
            this->_originY = originY;
            this->_originWidth = originWidth;
            this->_originHeight = originHeight;
        }

        void SetRenderToWorldSpace(bool renderToWorldSpace) noexcept
        {
            this->_renderToWorldSpace = renderToWorldSpace;
        }

        virtual void ReconfirmPixelSizes() noexcept
        {
        }

	protected:
	    int32_t _renderX = 0;
	    int32_t _renderY = 0;
	    uint32_t _width = 0;
	    uint32_t _height = 0;
	    RenderOriginPoints _renderOriginPoint = RenderOriginPoints::TopLeft;

	    uint32_t _originX = 0;
	    uint32_t _originY = 0;
	    uint32_t _originWidth = 0;
	    uint32_t _originHeight = 0;

	    bool _renderToWorldSpace = true;
	};
}

#endif
