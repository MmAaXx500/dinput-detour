# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("dinput-detour")

function(copy_binary_to_bin target)
    set(output_dir "${CMAKE_SOURCE_DIR}/out/bin")

    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${output_dir}"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${target}>" "${output_dir}/"
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_PDB_FILE:${target}>" "${output_dir}/"
        )
    endif()
endfunction()

function(enable_warns target)
    target_compile_options(${target} PRIVATE /W4 /WX)
endfunction()

file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}/compile_commands.json" SYMBOLIC)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DETOURS_BITS 64)
    set(BUILD_TOOLS True CACHE BOOL "Enable building tools (setdll, withdll)")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(DETOURS_BITS 32)
    set(BUILD_TOOLS False CACHE BOOL "Enable building tools (setdll, withdll)")
endif()

add_subdirectory("vendor/detours")

add_subdirectory("dinput-detour")

if(BUILD_TOOLS)
    add_subdirectory("withdll")
    add_subdirectory("setdll")
endif()
