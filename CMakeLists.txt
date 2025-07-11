# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

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

file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}/compile_commands.json" SYMBOLIC)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(DETOURS_BITS 64)
    set(DETOURS_TARGET_PROCESSOR "X64")
    set(BUILD_TOOLS True CACHE BOOL "Enable building tools (setdll, withdll)")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(DETOURS_BITS 32)
    set(DETOURS_TARGET_PROCESSOR "X86")
    set(BUILD_TOOLS False CACHE BOOL "Enable building tools (setdll, withdll)")
endif()

include(ExternalProject)
ExternalProject_Add(detours
    GIT_REPOSITORY "https://github.com/microsoft/Detours.git"
    GIT_TAG 9764ceb
    BUILD_IN_SOURCE true
    CONFIGURE_COMMAND ""
    BUILD_COMMAND 
        "cd" "src" "&&"
        "nmake" "/nologo" "DETOURS_TARGET_PROCESSOR=${DETOURS_TARGET_PROCESSOR}"
    INSTALL_COMMAND ""
    TEST_COMMAND ""
    BUILD_BYPRODUCTS
        "detours-prefix/src/detours/lib.X64/detours.lib"
        "detours-prefix/src/detours/lib.X86/detours.lib")

ExternalProject_Get_Property(detours SOURCE_DIR)
set(EXT_DETOURS_DIR ${SOURCE_DIR})
set(DETOURS_INCLUDE_DIRS "${EXT_DETOURS_DIR}/include")
if(DETOURS_BITS EQUAL 64)
    set(DETOURS_LIB_DIR "${EXT_DETOURS_DIR}/lib.X64")
else()
    set(DETOURS_LIB_DIR "${EXT_DETOURS_DIR}/lib.X86")
endif()
set(DETOURS_LIBRARY "${DETOURS_LIB_DIR}/detours.lib")

# Include sub-projects.
add_subdirectory("dinput-detour")

if(BUILD_TOOLS)
    add_subdirectory("withdll")
    add_subdirectory("setdll")
endif()
