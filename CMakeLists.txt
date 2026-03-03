# From https://github.com/brainboxdotcc/templatebot (slightly modified)

cmake_minimum_required (VERSION 3.15)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(PROJECT_NAME "dpptgg")

project(${PROJECT_NAME})
aux_source_directory("lib/dpptgg" dpptgg_src)
add_library(${PROJECT_NAME} SHARED ${dpptgg_src})

string(ASCII 27 Esc)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(DPP)
if(APPLE)
    if(CMAKE_APPLE_SILICON_PROCESSOR)
        set(OPENSSL_ROOT_DIR "/opt/homebrew/opt/openssl")
    else()
        set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
    endif()
    find_package(OpenSSL REQUIRED)
else()
    find_package(OpenSSL REQUIRED)
endif()

if(NOT CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(DL_LIBRARY dl)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    ${DL_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_CRYPTO_LIBRARY}
    ${OPENSSL_SSL_LIBRARY}
)

if (DPP_FOUND)
    target_link_libraries(${PROJECT_NAME} ${DPP_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${DPP_INCLUDE_DIR})
else()
    find_package(Git)
    message(WARNING "Could not find DPP install. Building from source instead.")
    if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                RESULT_VARIABLE RESULT)
        if (NOT RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to update the D++ submodule. Code: ${RESULT}.")
        else()
            add_subdirectory(DPP)
            target_link_libraries(${PROJECT_NAME} dpp)
        endif()
    else()
        message(FATAL_ERROR "Failed to find the git executable. Can't update the D++ submodule.")
    endif()
endif()
