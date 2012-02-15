project(multiload)

# building separately or as part of kdebase ?
if(NOT KDE4_FOUND)
   find_package(KDE4 REQUIRED)

   include_directories(
   ${CMAKE_CURRENT_BINARY_DIR}
   ${KDE4_INCLUDES} 
   )
endif(NOT KDE4_FOUND)

set(multiload_SRCS
    simpleplotter.cpp
    icongridlayout.cpp
    cpuload.cpp
    ramusage.cpp
    swapusage.cpp
    multiload.cpp)

#find_package(KDE4 REQUIRED)
kde4_add_ui_files(multiload_SRCS color-config.ui general-config.ui)
kde4_add_plugin(plasma_applet_multiload ${multiload_SRCS})
target_link_libraries(plasma_applet_multiload
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS plasma_applet_multiload
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES plasma-applet-multiload.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
