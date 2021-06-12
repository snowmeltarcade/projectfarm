set(V8_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/v8/include" CACHE INTERNAL "V8_INCLUDE_DIRS")

if (WIN32)
    set(V8_LIBRARIES "${LIBRARIES_ROOT_DIR}/v8/Windows/v8_libbase.lib;${LIBRARIES_ROOT_DIR}/v8/Windows/v8_libplatform.lib;${LIBRARIES_ROOT_DIR}/v8/Windows/v8_monolith.lib;winmm.lib;dbghelp.lib" CACHE INTERNAL "V8_LIBRARIES")
elseif (IOS)
    # we can include both the arm64 and x86_64 libs here as Xcode will choose the correct ones
    set(V8_LIBRARIES "${LIBRARIES_ROOT_DIR}/v8/ios/libv8_libbase.a;${LIBRARIES_ROOT_DIR}/v8/ios/libv8_libplatform.a;${LIBRARIES_ROOT_DIR}/v8/ios/libv8_monolith.a;${LIBRARIES_ROOT_DIR}/v8/ios_simulator/libv8_libbase.a;${LIBRARIES_ROOT_DIR}/v8/ios_simulator/libv8_libplatform.a;${LIBRARIES_ROOT_DIR}/v8/ios_simulator/libv8_monolith.a" CACHE INTERNAL "V8_LIBRARIES")
elseif (APPLE)
    set(V8_LIBRARIES "${LIBRARIES_ROOT_DIR}/v8/Darwin/libv8_libbase.a;${LIBRARIES_ROOT_DIR}/v8/Darwin/libv8_libplatform.a;${LIBRARIES_ROOT_DIR}/v8/Darwin/libv8_monolith.a" CACHE INTERNAL "V8_LIBRARIES")
else()
    set(V8_LIBRARIES "${LIBRARIES_ROOT_DIR}/v8/linux/libv8_libbase.a;${LIBRARIES_ROOT_DIR}/v8/linux/libv8_monolith.a" CACHE INTERNAL "V8_LIBRARIES")
endif()

# iOS doesn't support v8 pointer compression
if (NOT IOS)
    add_definitions(-DV8_31BIT_SMIS_ON_64BIT_ARCH -DV8_COMPRESS_POINTERS)
endif()