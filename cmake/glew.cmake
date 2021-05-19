if (MSVC)
    set(GLEW_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/glew/windows/include" CACHE INTERNAL "GLEW_INCLUDE_DIRS")
    set(GLEW_LIBRARIES "${LIBRARIES_ROOT_DIR}/windows/lib/Win64/glew32.lib" CACHE INTERNAL "GLEW_LIBRARIES")
    set(GLEW_DLL_PATH "${LIBRARIES_ROOT_DIR}/windows/bin/Win64/glew32.dll" CACHE INTERNAL "GLEW_DLL_PATH")
elseif(LINUX)
    set(GLEW_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/glew/linux/include" CACHE INTERNAL "GLEW_INCLUDE_DIRS")
    set(GLEW_LIBRARIES "${LIBRARIES_ROOT_DIR}/glew/linux/lib/libGLEWd.a" CACHE INTERNAL "GLEW_LIBRARIES")
    set(GLEW_DLL_PATH "" CACHE INTERNAL "GLEW_DLL_PATH")
else()
    set(GLEW_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/glew/darwin/include" CACHE INTERNAL "GLEW_INCLUDE_DIRS")
    set(GLEW_LIBRARIES "${LIBRARIES_ROOT_DIR}/glew/darwin/lib/libGLEW.a" CACHE INTERNAL "GLEW_LIBRARIES")
    set(GLEW_DLL_PATH "${LIBRARIES_ROOT_DIR}/glew/darwin/lib/libGLEW.dylib;${LIBRARIES_ROOT_DIR}/glew/lib/libGLEW.2.2.dylib;${LIBRARIES_ROOT_DIR}/glew/lib/libGLEW.2.2.0.dylib" CACHE INTERNAL "GLEW_DLL_PATH")
endif()

if (NOT LINUX)
    install(
        FILES
            ${GLEW_DLL_PATH}
        DESTINATION
            ${INSTALL_CLIENT_DIRECTORY}
    )

    install(
        FILES
            ${GLEW_DLL_PATH}
        DESTINATION
            ${INSTALL_SERVER_DIRECTORY}
    )

    install(
        FILES
            ${GLEW_DLL_PATH}
        DESTINATION
            ${INSTALL_LATEST_DIRECTORY}
    )
endif()