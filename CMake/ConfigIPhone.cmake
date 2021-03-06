function(config_target_if_iphone_build targetname is_library)   
	if (OGREKIT_BUILD_IPHONE)
		include_directories("include/iOS")
		set_target_properties(${targetname} PROPERTIES XCODE_ATTRIBUTE_GCC_UNROLL_LOOPS "YES")
		set_target_properties(${targetname} PROPERTIES XCODE_ATTRIBUTE_GCC_THUMB_SUPPORT "NO")
		set_target_properties(${targetname} PROPERTIES XCODE_ATTRIBUTE_GCC_PRECOMPILE_PREFIX_HEADER "YES")
		set_target_properties(${targetname} PROPERTIES XCODE_ATTRIBUTE_SYMROOT "${CMAKE_BINARY_DIR}/bin")
		#set_target_properties(${targetname} PROPERTIES XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "iOS 5.1")
		#set_target_properties(${targetname} PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "iPhone/iPad")
		if (is_library)
			#set_target_properties(${targetname} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "bin$(EFFECTIVE_PLATFORM_NAME)")
		else()
			#set_target_properties(${targetname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "bin$(EFFECTIVE_PLATFORM_NAME)")
			set_target_properties(${targetname} PROPERTIES XCODE_ATTRIBUTE_LIBRARY_SEARCH_PATHS "$(CONFIGURATION_BUILD_DIR)")
			set_target_properties(${targetname} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
		endif()
	endif()
endfunction()
