set(DETOUR_LIBNAME "dinput-detour${DETOURS_BITS}")
add_library(${DETOUR_LIBNAME} SHARED 
    "dinput-detour.cpp"
    "utils.cpp"
    "log.cpp"
    "direct_input_8.cpp"
    "direct_input_device_8.cpp")

target_link_options(${DETOUR_LIBNAME} PRIVATE 
    /DEF:${CMAKE_CURRENT_SOURCE_DIR}/dinput-detour.def
)

enable_warns(${DETOUR_LIBNAME})

copy_binary_to_bin(${DETOUR_LIBNAME})

target_link_libraries(${DETOUR_LIBNAME} PRIVATE detours Dinput8.lib DxGUID.lib user32.lib)
