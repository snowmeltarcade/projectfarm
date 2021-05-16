#ifndef PROJECTFARM_GC_PERSISTENT_H
#define PROJECTFARM_GC_PERSISTENT_H

#include <vector>
#include <mutex>
#include <v8.h>

namespace projectfarm::shared::scripting
{
    // This should be a base class of an object which will be used as
    // an object template in v8, that needs to be new`d up and then
    // later deleted.
    class GCPersistent
    {
    public:
        GCPersistent() = default;
        virtual ~GCPersistent() = default;

        // This requests that this object will be deleted if v8 finalizes this object
        // NOTE: Only call this function if this object is allocated by `new` and will
        // not be deleted some other way. If v8 does not finalize this object, it will
        // be put in a deletion queue to be deleted by the script system on shutdown
        template <class T>
        void SetGCPersistent(v8::Isolate* isolate,
                             v8::Local<v8::Object> objectInstance) noexcept
        {
            auto deleteThisCallBack = [](const v8::WeakCallbackInfo<T> &info)
            {
                // get the base object
                auto gcPersistent = static_cast<GCPersistent*>(info.GetParameter());

                // get the derived object to delete
                auto objToDelete = static_cast<T*>(info.GetParameter());

                GCPersistent::RemoveFromDeleteQueue(gcPersistent);

                delete objToDelete;
            };

            // associate the local with this persistent
            this->_handle.Reset(isolate, objectInstance);

            // apply for `this` to be deleted on finalization
            this->_handle.SetWeak(static_cast<T*>(this), deleteThisCallBack, v8::WeakCallbackType::kParameter);

            GCPersistent::AddToDeleteQueue(this);
        }

        static void ClearDeleteQueue() noexcept;

    private:
        v8::Persistent<v8::Object, v8::CopyablePersistentTraits<v8::Object>> _handle;

        static std::mutex _deleteQueueMutex;
        static std::vector<GCPersistent*> _deleteQueue;

        static void AddToDeleteQueue(GCPersistent* obj) noexcept;
        static void RemoveFromDeleteQueue(GCPersistent* obj) noexcept;
    };
}

#endif