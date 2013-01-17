# ---------------------------------------------------------
cmake_minimum_required(VERSION 2.6)



include_directories(${OGRELITE_SOURCE_DIR}/OgreMain/include/OSX)
SET(EXETYPE MACOSX_BUNDLE)
SET_SOURCE_FILES_PROPERTIES(MainMenu.nib    PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
SET_SOURCE_FILES_PROPERTIES(momo_ogre_i.blend PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
SET_SOURCE_FILES_PROPERTIES(multitouch_test.blend PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
SET_SOURCE_FILES_PROPERTIES(assets.blend PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
SET_SOURCE_FILES_PROPERTIES(momo_ogre_i_mini.blend PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
SET_SOURCE_FILES_PROPERTIES(icon.png PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
SET_SOURCE_FILES_PROPERTIES(icon@2x.png PROPERTIES MACOSX_PACKAGE_LOCATION Resources )
SET_SOURCE_FILES_PROPERTIES(icon-72.png PROPERTIES MACOSX_PACKAGE_LOCATION Resources )

SET(NibFile MainMenu.nib)


set(SRC 
	momo_ogre_i.blend
	multitouch_test.blend
        momo_ogre_i_mini.blend
	assets.blend
	Main.mm

)

include_directories(
	${OGREKIT_INCLUDE}
	${OGREKIT_LIBROCKET_INCLUDE}
)

set(HiddenCMakeLists ../CMakeLists.txt)
source_group(ParentCMakeLists FILES ${HiddenCMakeLists})

add_executable(
	IPhoneDemo MACOSX_BUNDLE
	MainMenu.nib
	icon.png
	icon@2x.png
	icon-72.png
	${SRC} 
	${HiddenCMakeLists}
	)

IF(OGREKIT_BUILD_GLES2RS)
   set(alllibs "${OGREKIT_LIB};${OGREKIT_LIBROCKET_LIBS};${OGREKIT_BULLET_LIBS};${OGREKIT_GLES2RS_LIBS};${OGREKIT_FPX_LIBS};")
ELSE()
   set(alllibs "${OGREKIT_LIB};${OGREKIT_LIBROCKET_LIBS};${OGREKIT_BULLET_LIBS};${OGREKIT_GLESRS_LIBS};${OGREKIT_FPX_LIBS};")
ENDIF()

message("${alllibs}")
set(translibs "")
set(tdeplibs "")
STRING(REGEX MATCHALL "([^;/]*);" deplibs "${alllibs}") 
FOREACH(libstr ${deplibs})
   STRING(REGEX MATCH "(.framework)" flib "${libstr}")
   IF (NOT (flib STREQUAL ".framework"))
      set(translibs "${translibs}-l${libstr} ")
      set(tdeplibs "${tdeplibs}${libstr} ")
   ENDIF()
ENDFOREACH()
set(translibs "-framework OpenGLES -framework Foundation -framework CoreGraphics -framework QuartzCore -framework UIKit -framework OpenAL -Wl,-search_paths_first -Wl,-headerpad_max_install_names -lz ${translibs}")
message("${translibs}")
message("${tdeplibs}")
#target_link_libraries(IPhoneDemo ${tdeplibs})
set_target_properties(IPhoneDemo PROPERTIES XCODE_ATTRIBUTE_OTHER_LDFLAGS "${translibs}")
add_dependencies(IPhoneDemo ${tdeplibs})
set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.\${PRODUCT_NAME:rfc1034identifier}")
set_property(TARGET IPhoneDemo PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)

config_target_if_iphone_build(IPhoneDemo FALSE)
