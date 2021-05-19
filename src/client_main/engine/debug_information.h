#ifndef PROJECTFARM_DEBUG_INFORMATION_H
#define PROJECTFARM_DEBUG_INFORMATION_H

#include <cstdint>

namespace projectfarm::engine
{
    class DebugInformation final
    {
    public:
        DebugInformation() = default;
        virtual ~DebugInformation() = default;

        void Reset();

        [[nodiscard]] uint64_t GetLastFrameDrawCalls() const
        {
            return this->_lastFrameDrawCalls;
        }

        [[nodiscard]] uint64_t GetLastFrameTilesDrawn() const
        {
            return this->_lastFrameTilesDrawn;
        }

        [[nodiscard]] uint64_t GetNumberOfTexturesLoaded() const
        {
            return this->_numberOfTexturesLoaded;
        }

#ifdef DEBUG
        void AddDrawCall(uint64_t drawCallsToAdd = 1)
#else
        void AddDrawCall(uint64_t = 0)
#endif
        {
#ifdef DEBUG
            this->_drawCalls += drawCallsToAdd;
#endif
        }

#ifdef DEBUG
        void AddDrawnTile(uint64_t tilesDrawnToAdd = 1)
#else
        void AddDrawnTile(uint64_t = 0)
#endif
        {
#ifdef DEBUG
            this->_tilesDrawn += tilesDrawnToAdd;
#endif
        }

#ifdef DEBUG
        void AddNumberOfTexturesLoaded(uint64_t numberOfTexturesLoaded = 1)
#else
        void AddNumberOfTexturesLoaded(uint64_t = 0)
#endif
        {
#ifdef DEBUG
            this->_numberOfTexturesLoaded += numberOfTexturesLoaded;
#endif
        }

        void OnFrame();

    private:
#ifdef DEBUG
        uint64_t _tilesDrawn = 0;
        uint64_t _drawCalls = 0;
#endif

        uint64_t _lastFrameTilesDrawn = 0;
        uint64_t _lastFrameDrawCalls = 0;
        uint64_t _numberOfTexturesLoaded = 0;
    };
}

#endif
