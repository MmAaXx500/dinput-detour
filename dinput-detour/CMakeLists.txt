# CMakeList.txt : CMake project for dinput-detour, include source and define
# project specific logic here.
#

# Add source to this project's executable.
set(DETOUR_LIBNAME "dinput-detour${DETOURS_BITS}")
add_library(${DETOUR_LIBNAME} SHARED 
  "dinput-detour.cpp"
  "utils.cpp"
  "log.cpp"
  "direct_input_8.cpp"
  "direct_input_device_8.cpp")
copy_binary_to_bin(${DETOUR_LIBNAME})

add_dependencies(${DETOUR_LIBNAME} detours)
target_include_directories(${DETOUR_LIBNAME} PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(${DETOUR_LIBNAME} PRIVATE ${DETOURS_LIBRARY} Dinput8.lib DxGUID.lib user32.lib)
set_target_properties(${DETOUR_LIBNAME} PROPERTIES LINK_OPTIONS "/EXPORT:DetourFinishHelperProcess,@1,NONAME")


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${DETOUR_LIBNAME} PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
