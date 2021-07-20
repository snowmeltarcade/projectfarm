#ifndef PROJECTFARM_SERVICE_H
#define PROJECTFARM_SERVICE_H

#include "iwindow.h"

namespace projectfarm::shared::api::windowing
{
    // used for running as a service/daemon
    class Service : public IWindow
    {
    };
}

#endif
