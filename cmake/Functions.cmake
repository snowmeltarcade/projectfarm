function(CopyToData FILE_EXT FROM TO)
    file(GLOB files "${CMAKE_CURRENT_LIST_DIR}/${FROM}/*.${FILE_EXT}")

    foreach (file ${files})
        get_filename_component(filename ${file} NAME)

        configure_file("${FROM}${filename}" "${FROM}${filename}" COPYONLY)

        install(
            FILES
                ${file}
            DESTINATION
                "${INSTALL_CLIENT_DIRECTORY}/data/${TO}"
        )

        install(
                FILES
                ${file}
                DESTINATION
                "${INSTALL_SERVER_DIRECTORY}/data/${TO}"
        )

        install(
                FILES
                ${file}
                DESTINATION
                "${INSTALL_LATEST_DIRECTORY}/data/${TO}"
        )
    endforeach()
endfunction()