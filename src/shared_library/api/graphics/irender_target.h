#ifndef PROJECTFARM_IRENDER_TARGET_H
#define PROJECTFARM_IRENDER_TARGET_H

namespace projectfarm::shared::api::graphics
{
    class IRenderTarget
    {
    public:
        IRenderTarget() = default;
        virtual ~IRenderTarget() = default;
        IRenderTarget(const IRenderTarget&) = delete;
        IRenderTarget(IRenderTarget&&) = default;
    };
}

#endif
