#ifndef PROJECTFARM_ICAMERA_H
#define PROJECTFARM_ICAMERA_H

namespace projectfarm::shared::api::graphics
{
    class ICamera
    {
    public:
        ICamera() = default;
        virtual ~ICamera() = default;
        ICamera(const ICamera&) = delete;
        ICamera(ICamera&&) = default;
    };
}

#endif
