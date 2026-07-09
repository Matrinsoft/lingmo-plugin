set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Test)

function(add_lingmo_plugin_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name}
        PRIVATE
            Lingmo::Plugin
            Qt6::Test
    )
    target_compile_definitions(${name}
        PRIVATE
            TESTDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    )
    add_test(NAME ${name} COMMAND ${name})
endfunction()

add_lingmo_plugin_test(tst_PluginMetadata)
add_lingmo_plugin_test(tst_PluginManager)
