# the build for Windows X64
project(zterm-win64 CXX)

set(ZTERMAPP_DIR "${CMAKE_CURRENT_LIST_DIR}"
    CACHE PATH "ZTerm application directory")

set(PRECOMPILE_HEADER_FILES stdafx.h)

add_executable(${PROJECT_NAME} WIN32 
	ZTerm.cpp
	ZTerm.rc
	)

# Specify the precompiled header(s)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PRECOMPILE_HEADER_FILES})

set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Specify the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/wtl ${CMAKE_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} libwt)

