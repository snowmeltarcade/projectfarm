file(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/*.*")

foreach (file ${files})
    get_filename_component(filename ${file} NAME)

    configure_file(${filename} ${filename} COPYONLY)
endforeach()

add_subdirectory("config")
add_subdirectory("scripting")
add_subdirectory("css")
