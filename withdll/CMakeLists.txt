add_executable (withdll "withdll.cpp")
copy_binary_to_bin(withdll)

target_link_libraries(withdll PRIVATE detours)
