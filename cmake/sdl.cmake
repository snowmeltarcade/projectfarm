if (WIN32)
    set(SDL2_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl/windows/include/SDL2" CACHE INTERNAL "SDL2_INCLUDE_DIRS")
    set(SDL2_NET_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_net/windows/include/SDL2" CACHE INTERNAL "SDL2_NET_INCLUDE_DIRS")
    set(SDL2_IMG_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_image/windows/include/SDL2" CACHE INTERNAL "SDL2_IMG_INCLUDE_DIRS")
    set(SDL2_TTF_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_ttf/windows/include/SDL2" CACHE INTERNAL "SDL2_TTF_INCLUDE_DIRS")
elseif (LINUX)
    set(SDL2_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl/linux/include/SDL2" CACHE INTERNAL "SDL2_INCLUDE_DIRS")
    set(SDL2_NET_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_net/linux/include/SDL2" CACHE INTERNAL "SDL2_NET_INCLUDE_DIRS")
    set(SDL2_IMG_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_image/linux/include/SDL2" CACHE INTERNAL "SDL2_IMG_INCLUDE_DIRS")
    set(SDL2_TTF_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_ttf/linux/include/SDL2" CACHE INTERNAL "SDL2_TTF_INCLUDE_DIRS")
else()
    set(SDL2_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl/darwin/include/SDL2" CACHE INTERNAL "SDL2_INCLUDE_DIRS")
    set(SDL2_NET_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_net/darwin/include/SDL2" CACHE INTERNAL "SDL2_NET_INCLUDE_DIRS")
    set(SDL2_IMG_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_image/darwin/include/SDL2" CACHE INTERNAL "SDL2_IMG_INCLUDE_DIRS")
    set(SDL2_TTF_INCLUDE_DIRS "${LIBRARIES_ROOT_DIR}/sdl_ttf/darwin/include/SDL2" CACHE INTERNAL "SDL2_TTF_INCLUDE_DIRS")
endif()

if (WIN32)
    # Only support 64-bit
    if (${CMAKE_SIZEOF_VOID_P} MATCHES 8)
        set(SDL2_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl/windows/SDL2main.lib;${LIBRARIES_ROOT_DIR}/sdl/windows/SDL2.lib" CACHE INTERNAL "SDL2_LIBRARIES")
        set(SDL2_NET_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_net/windows/SDL2_net.lib" CACHE INTERNAL "SDL2_NET_LIBRARIES")
        set(SDL2_IMG_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_image/windows/SDL2_image.lib" CACHE INTERNAL "SDL2_IMG_LIBRARIES")
        set(SDL2_TTF_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_ttf/windows/SDL2_ttf.lib" CACHE INTERNAL "SDL2_TTF_LIBRARIES")

        set(SDL2_DLL_PATH "${LIBRARIES_ROOT_DIR}/sdl/windows/SDL2.dll" CACHE INTERNAL "SDL2_DLL_PATH")
        set(SDL2_NET_DLL_PATH "${LIBRARIES_ROOT_DIR}/sdl_net/windows/SDL2_net.dll" CACHE INTERNAL "SDL2_NET_DLL_PATH")
        set(SDL2_IMG_DLL_PATH "${LIBRARIES_ROOT_DIR}/sdl_image/windows/SDL2_image.dll;${LIBRARIES_ROOT_DIR}/sdl_image/windows/libjpeg-9.dll;${LIBRARIES_ROOT_DIR}/sdl_image/windows/libpng16-16.dll;${LIBRARIES_ROOT_DIR}/sdl_image/windows/libtiff-5.dll;${LIBRARIES_ROOT_DIR}/sdl_image/windows/libwebp-7.dll;${LIBRARIES_ROOT_DIR}/sdl_image/windows/zlib1.dll" CACHE INTERNAL "SDL2_IMG_DLL_PATH")
        set(SDL2_TTF_DLL_PATH "${LIBRARIES_ROOT_DIR}/sdl_ttf/windows/SDL2_ttf.dll;${LIBRARIES_ROOT_DIR}/sdl_ttf/windows/libfreetype-6.dll" CACHE INTERNAL "SDL2_TTF_DLL_PATH")
    else()
        message(ERROR "32-bit is not supported.")
    endif()
elseif (LINUX)
    set(OpenGL_GL_PREFERENCE "GLVND")
    
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

    find_package(SDL2 REQUIRED)
    find_package(SDL2_net REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)

    set(SDL2_LIBRARIES "${SDL2_LIBRARIES}" CACHE INTERNAL "SDL2_LIBRARIES")
    set(SDL2_NET_LIBRARIES "${SDL2_NET_LIBRARIES}" CACHE INTERNAL "SDL2_NET_LIBRARIES")
    set(SDL2_IMAGE_LIBRARIES "${SDL2_IMAGE_LIBRARIES}" CACHE INTERNAL "SDL2_IMAGE_LIBRARIES")
    set(SDL2_TTF_LIBRARIES "${SDL2_TTF_LIBRARIES}" CACHE INTERNAL "SDL2_TTF_LIBRARIES")
else()
    # These SDL linker flags were gleaned by running `sdl2-config --static-libs` locally
    set(SDL2_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl/darwin/libSDL2.a;-lm -liconv -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-weak_framework,CoreHaptics -Wl,-weak_framework,GameController -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal" CACHE INTERNAL "SDL2_LIBRARIES")
    set(SDL2_NET_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_net/darwin/libSDL2_net.a" CACHE INTERNAL "SDL2_NET_LIBRARIES")
    set(SDL2_IMG_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_image/darwin/libSDL2_image.a" CACHE INTERNAL "SDL2_IMG_LIBRARIES")
    set(SDL2_TTF_LIBRARIES "${LIBRARIES_ROOT_DIR}/sdl_ttf/darwin//libSDL2_ttf.a" CACHE INTERNAL "SDL2_TTF_LIBRARIES")
endif()

string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
string(STRIP "${SDL2_NET_LIBRARIES}" SDL2_NET_LIBRARIES)
string(STRIP "${SDL2_IMG_LIBRARIES}" SDL2_IMG_LIBRARIES)
string(STRIP "${SDL2_TTF_LIBRARIES}" SDL2_TTF_LIBRARIES)

if (NOT LINUX)
    install(
        FILES
            ${SDL2_DLL_PATH}
            ${SDL2_NET_DLL_PATH}
            ${SDL2_IMG_DLL_PATH}
            ${SDL2_TTF_DLL_PATH}
        DESTINATION
            ${INSTALL_CLIENT_DIRECTORY}
    )

    install(
        FILES
            ${SDL2_DLL_PATH}
            ${SDL2_NET_DLL_PATH}
            ${SDL2_IMG_DLL_PATH}
            ${SDL2_TTF_DLL_PATH}
        DESTINATION
            ${INSTALL_SERVER_DIRECTORY}
    )

    install(
        FILES
            ${SDL2_DLL_PATH}
            ${SDL2_NET_DLL_PATH}
            ${SDL2_IMG_DLL_PATH}
            ${SDL2_TTF_DLL_PATH}
        DESTINATION
            ${INSTALL_LATEST_DIRECTORY}
    )
endif()