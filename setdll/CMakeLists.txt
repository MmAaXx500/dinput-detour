# CMakeList.txt : CMake project for dinput-detour, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (setdll "setdll.cpp")
copy_binary_to_bin(setdll)

target_link_libraries(setdll PRIVATE detours)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET setdll PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
