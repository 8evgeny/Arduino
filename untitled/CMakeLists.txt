cmake_minimum_required (VERSION 3.15)

project(untitled VERSION 0.0.1 LANGUAGES C CXX ASM ASM_MASM)

find_package(Qul)

if(Qul_VERSION VERSION_GREATER_EQUAL "1.7")
    qul_add_target(untitled)
else()
    add_executable(untitled)
    target_link_libraries(untitled
        Qul::QuickUltralite
        Qul::QuickUltralitePlatform)
endif()

qul_target_qml_sources(untitled untitled.qml)
app_target_setup_os(untitled)

if(Qul_VERSION VERSION_GREATER_EQUAL "2.0")
    app_target_default_entrypoint(untitled untitled)
else()
    app_target_default_main(untitled untitled)
endif()
