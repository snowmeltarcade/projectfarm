set(V8_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/v8/include" CACHE INTERNAL "V8_INCLUDE_DIRS")

if (WIN32)
    set(V8_LIBRARIES "${LIBRARIES_ROOT_DIR}/v8/Windows/v8_libbase.lib;${LIBRARIES_ROOT_DIR}/v8/Windows/v8_libplatform.lib;${LIBRARIES_ROOT_DIR}/v8/Windows/v8_monolith.lib;winmm.lib;dbghelp.lib" CACHE INTERNAL "V8_LIBRARIES")
elseif (APPLE)
    set(V8_LIBRARIES "${LIBRARIES_ROOT_DIR}/v8/Darwin/libv8_libbase.a;${LIBRARIES_ROOT_DIR}/v8/Darwin/libv8_libplatform.a;${LIBRARIES_ROOT_DIR}/v8/Darwin/libv8_monolith.a" CACHE INTERNAL "V8_LIBRARIES")
else()
    set(V8_LIBRARIES "${LIBRARIES_ROOT_DIR}/v8/Linux/libv8_libbase.a;${LIBRARIES_ROOT_DIR}/v8/Linux/libv8_monolith.a" CACHE INTERNAL "V8_LIBRARIES")
endif()