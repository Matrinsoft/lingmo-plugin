set(PUBLIC_HEADERS
    ${CMAKE_SOURCE_DIR}/include/LingmoPlugin/PluginMetadata.h
    ${CMAKE_SOURCE_DIR}/include/LingmoPlugin/PluginLoader.h
    ${CMAKE_SOURCE_DIR}/include/LingmoPlugin/PluginManager.h
)

set(PRIVATE_HEADERS
    LingmoPlugin/private/PluginLoader_p.h
    LingmoPlugin/private/PluginManager_p.h
)

set(SOURCES
    LingmoPlugin/PluginMetadata.cpp
    LingmoPlugin/PluginLoader.cpp
    LingmoPlugin/PluginManager.cpp
)

set(CMAKE_AUTOMOC ON)

add_library(LingmoPlugin SHARED ${SOURCES} ${PUBLIC_HEADERS} ${PRIVATE_HEADERS})
add_library(Lingmo::Plugin ALIAS LingmoPlugin)

target_include_directories(LingmoPlugin
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(LingmoPlugin
    PUBLIC
        Qt6::Core
)

target_compile_definitions(LingmoPlugin
    PRIVATE
        LINGMOPLUGIN_LIBRARY
)

set_target_properties(LingmoPlugin PROPERTIES
    SOVERSION 1
    VERSION ${PROJECT_VERSION}
    DEFINE_SYMBOL LINGMOPLUGIN_LIBRARY
    OUTPUT_NAME LingmoPlugin
)

configure_file(
    "${CMAKE_SOURCE_DIR}/include/LingmoPlugin/LingmoPluginExport.h.in"
    "${CMAKE_BINARY_DIR}/include/LingmoPlugin/LingmoPluginExport.h"
)

configure_file(
    "${CMAKE_SOURCE_DIR}/include/LingmoPlugin/LingmoPluginVersion.h.in"
    "${CMAKE_BINARY_DIR}/include/LingmoPlugin/LingmoPluginVersion.h"
)

install(TARGETS LingmoPlugin
    EXPORT LingmoPluginTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/LingmoPlugin/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LingmoPlugin
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*.h.in" EXCLUDE
)

install(FILES
    "${CMAKE_BINARY_DIR}/include/LingmoPlugin/LingmoPluginExport.h"
    "${CMAKE_BINARY_DIR}/include/LingmoPlugin/LingmoPluginVersion.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/LingmoPlugin
)

install(EXPORT LingmoPluginTargets
    FILE LingmoPluginTargets.cmake
    NAMESPACE Lingmo::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LingmoPlugin
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/LingmoPluginConfig.cmake.in"
    "${CMAKE_BINARY_DIR}/LingmoPluginConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LingmoPlugin
    PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)

write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/LingmoPluginConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    "${CMAKE_BINARY_DIR}/LingmoPluginConfig.cmake"
    "${CMAKE_BINARY_DIR}/LingmoPluginConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/LingmoPlugin
)
