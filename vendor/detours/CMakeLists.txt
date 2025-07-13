set(DETOURS_ROOT "src")
set(DETOURS_SRC "${DETOURS_ROOT}/src")
set(DETOURS_INCLUDE "${DETOURS_ROOT}/include")

add_library(detours STATIC
    "${DETOURS_SRC}/detver.h"
    "${DETOURS_SRC}/detours.h"
    "${DETOURS_SRC}/detours.cpp"
    "${DETOURS_SRC}/modules.cpp"
    "${DETOURS_SRC}/disasm.cpp"
    "${DETOURS_SRC}/image.cpp"
    "${DETOURS_SRC}/creatwth.cpp"
    "${DETOURS_SRC}/disolx86.cpp"
    "${DETOURS_SRC}/disolx64.cpp"
    "${DETOURS_SRC}/disolia64.cpp"
    "${DETOURS_SRC}/disolarm.cpp"
    "${DETOURS_SRC}/disolarm64.cpp"
    "${DETOURS_SRC}/uimports.cpp"
)

set_property(
    SOURCE "src/src/uimports.cpp"
    APPEND PROPERTY HEADER_FILE_ONLY true)

file(COPY
    ${DETOURS_SRC}/detours.h
    ${DETOURS_SRC}/detver.h
    DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/${DETOURS_INCLUDE}")

target_include_directories(detours INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/${DETOURS_INCLUDE}")
