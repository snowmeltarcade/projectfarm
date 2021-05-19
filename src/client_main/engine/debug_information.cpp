#include "debug_information.h"

namespace projectfarm::engine
{
    void DebugInformation::Reset()
    {
#ifdef DEBUG
        this->_drawCalls = 0;
        this->_lastFrameDrawCalls = 0;

        this->_tilesDrawn = 0;
        this->_lastFrameTilesDrawn = 0;
#endif
    }

    void DebugInformation::OnFrame()
    {
#ifdef DEBUG
        this->_lastFrameDrawCalls = this->_drawCalls;
        this->_drawCalls = 0;

        this->_lastFrameTilesDrawn = this->_tilesDrawn;
        this->_tilesDrawn = 0;
#endif
    }
}