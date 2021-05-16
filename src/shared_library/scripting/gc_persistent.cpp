#include <algorithm>

#include "gc_persistent.h"
namespace projectfarm::shared::scripting
{
    std::mutex GCPersistent::_deleteQueueMutex;
    std::vector<GCPersistent*> GCPersistent::_deleteQueue;

    void GCPersistent::ClearDeleteQueue() noexcept
    {
        for (auto item : GCPersistent::_deleteQueue)
        {
            delete item;
        }

        GCPersistent::_deleteQueue.clear();
    }

    void GCPersistent::AddToDeleteQueue(GCPersistent* obj) noexcept
    {
        std::scoped_lock l(GCPersistent::_deleteQueueMutex);

        GCPersistent::_deleteQueue.push_back(obj);
    }

    void GCPersistent::RemoveFromDeleteQueue(GCPersistent* obj) noexcept
    {
        std::scoped_lock l(GCPersistent::_deleteQueueMutex);

        auto iter = std::find(GCPersistent::_deleteQueue.begin(),
                              GCPersistent::_deleteQueue.end(),
                              obj);

        if (iter == GCPersistent::_deleteQueue.end())
        {
            return;
        }

        GCPersistent::_deleteQueue.erase(iter);
    }
}