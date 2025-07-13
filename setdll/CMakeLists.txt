add_executable (setdll "setdll.cpp")
copy_binary_to_bin(setdll)

target_link_libraries(setdll PRIVATE detours)
