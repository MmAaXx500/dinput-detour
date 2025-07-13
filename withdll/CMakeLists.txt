# CMakeList.txt : CMake project for dinput-detour, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (withdll "withdll.cpp")
copy_binary_to_bin(withdll)

target_link_libraries(withdll PRIVATE detours)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET withdll PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
