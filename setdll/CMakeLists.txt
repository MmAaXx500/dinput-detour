add_executable (setdll "setdll.cpp")
copy_binary_to_bin(setdll)

if(MINGW)
    # static to avoid missing libstdc++-6.dll, libwinpthread-1.dll, etc
    target_link_options(setdll PRIVATE -static)
endif()

target_link_libraries(setdll PRIVATE detours)
