function(SetupiOSBuild PROJECT_NAME IDENTIFIER NAME)
    get_property(tmpfrom GLOBAL PROPERTY IOS_RESOURCE_FILES_FROM)
	get_property(tmpto GLOBAL PROPERTY IOS_RESOURCE_FILES_TO)

	foreach (from to IN ZIP_LISTS tmpfrom tmpto)
		set_source_files_properties(
			${from}
			PROPERTIES
			MACOSX_PACKAGE_LOCATION Resources/data/${to}
		)

		target_sources(
			"${PROJECT_NAME}"
			PUBLIC
			${from}
		)
	endforeach()

	set_target_properties(
		"${PROJECT_NAME}"
		PROPERTIES
			MACOSX_BUNDLE TRUE
			MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
			XCODE_ATTRIBUTE_BUNDLE_IDENTIFIER "com.snowmeltarcade.projectfarm.${IDENTIFIER}"
			XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.snowmeltarcade.projectfarm.${IDENTIFIER}"
			XCODE_ATTRIBUTE_PRODUCT_NAME "Project Farm ${NAME}"
			XCODE_ATTRIBUTE_BUNDLE_NAME "Project Farm ${NAME}"
			XCODE_ATTRIBUTE_EXECUTABLE_NAME "projectfarm${IDENTIFIER}"
	)

	configure_file("Info.plist.in" "Info.plist")
endfunction()