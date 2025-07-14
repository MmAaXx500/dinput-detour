add_executable (withdll "withdll.cpp")
copy_binary_to_bin(withdll)

if(MINGW)
    # static to avoid missing libstdc++-6.dll, libwinpthread-1.dll, etc
    target_link_options(withdll PRIVATE -static)
endif()

target_link_libraries(withdll PRIVATE detours)
