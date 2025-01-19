# Basic project requirements
cmake_minimum_required(VERSION 3.30)
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CMAKE_OSX_SYSROOT macosx)
else(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
    set(VCPKG_TARGET_TRIPLET "x64-mingw-static")
endif()
project(CanvasView)


# Set CMake variables
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Main executable
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    add_executable(CanvasView MACOSX_BUNDLE main.cpp buildfiles/icon.icns)
else(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_executable(CanvasView  main.cpp buildfiles/wicon.rc) # WIN32
endif()


target_compile_options(CanvasView PRIVATE -fpermissive)


if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # Set macos executable rpath and result path
    set_target_properties(CanvasView PROPERTIES INSTALL_RPATH "@executable_path/../Resources" BUILD_WITH_INSTALL_RPATH TRUE)
    set_target_properties(CanvasView PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
    set_target_properties(CanvasView PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/CanvasView.app/Contents/Resources)


    # Link libraries and include directories
    find_package(CURL REQUIRED)
    find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3-shared)
    find_package(SDL3_TTF REQUIRED CONFIG COMPONENTS SDL3_ttf-shared)
    target_link_libraries(CanvasView PRIVATE CURL::libcurl)
    target_link_libraries(CanvasView PRIVATE SDL3::SDL3)
    target_link_libraries(CanvasView PRIVATE SDL3_ttf::SDL3_ttf)


    # Make directories for macos app
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/CanvasView.app/Contents/MacOS)
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/build/CanvasView.app/Contents/Resources)


    # Add basic files to macos app
    file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/info.plist DESTINATION ${CMAKE_SOURCE_DIR}/build/CanvasView.app/Contents)
    file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/icon.icns DESTINATION ${CMAKE_SOURCE_DIR}/build/CanvasView.app/Contents/Resources)
    file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/Font.ttf DESTINATION ${CMAKE_SOURCE_DIR}/build/CanvasView.app/Contents/Resources)


    # Add resources to macos app
    add_custom_command(TARGET CanvasView POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:CanvasView>/../Resources)
    add_custom_command(TARGET CanvasView POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared> $<TARGET_FILE_DIR:CanvasView>/../Resources)
    add_custom_command(TARGET CanvasView POST_BUILD COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE_DIR:CanvasView>/../Resources/libSDL3_ttf.0.0.0.dylib $<TARGET_FILE_DIR:CanvasView>/../Resources/libSDL3_ttf.0.dylib)



else(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Set windows result path
    set_target_properties(CanvasView PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build/CanvasView")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/CanvasView")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/build/CanvasView")
    set(CMAKE_PREFIX_PATH "C:/lib")


    # Link libraries and include directories
    set(CURL_DIR "$ENV{VCPKG_ROOT}/installed/x64-mingw-static/share/curl")
    find_package(CURL REQUIRED)
    find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3-shared)
    find_package(SDL3_TTF REQUIRED CONFIG COMPONENTS SDL3_ttf-shared)
    target_link_libraries(CanvasView PRIVATE CURL::libcurl)
    target_link_libraries(CanvasView PRIVATE SDL3::SDL3)
    target_link_libraries(CanvasView PRIVATE SDL3_ttf::SDL3_ttf)
    #target_include_directories(CanvasView PRIVATE C:/usr/local/include)
    #target_include_directories(CanvasView PRIVATE C:/Users/ChedRed/Desktop/None/Coding/Projects/Libraries/vcpkg/installed/x64-windows/include)


    # Add basic files to windows app
    file(COPY ${CMAKE_SOURCE_DIR}/buildfiles/Font.ttf DESTINATION ${CMAKE_SOURCE_DIR}/build/CanvasView)


    # Add resources to windows app
    file(COPY C:/usr/local/lib/SDL3.dll DESTINATION "${CMAKE_SOURCE_DIR}/build/CanvasView")
    file(COPY C:/usr/local/lib/SDL3_ttf.dll DESTINATION "${CMAKE_SOURCE_DIR}/build/CanvasView")
    add_custom_command(TARGET CanvasView POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:CanvasView>)
    add_custom_command(TARGET CanvasView POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared> $<TARGET_FILE_DIR:CanvasView>)
endif()
