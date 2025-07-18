set(DETOUR_LIBNAME "dinput-detour${DETOURS_BITS}")
add_library(${DETOUR_LIBNAME} SHARED 
    "dinput-detour.cpp"
    "utils.cpp"
    "log.cpp"
    "stringconv.cpp"
    "direct_input_8.cpp"
    "direct_input_device_8.cpp")

if(MSVC)
    target_link_options(${DETOUR_LIBNAME} PRIVATE 
        /DEF:${CMAKE_CURRENT_SOURCE_DIR}/dinput-detour.def
    )
else()
    target_sources(${DETOUR_LIBNAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/dinput-detour.def
    )
    # static to avoid missing libstdc++-6.dll, libwinpthread-1.dll, etc
    target_link_options(${DETOUR_LIBNAME} PRIVATE -static)
endif()

set_target_properties(${DETOUR_LIBNAME} PROPERTIES PREFIX "")

enable_warns(${DETOUR_LIBNAME})

copy_binary_to_bin(${DETOUR_LIBNAME})

# Keep libs lowercase, so mingw finds it
target_link_libraries(${DETOUR_LIBNAME} PRIVATE detours dinput8.lib dxguid.lib user32.lib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${DETOUR_LIBNAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
