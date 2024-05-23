project(AtomicMem)


file (GLOB_RECURSE PHYSICS_SOURCES "src/**.cpp")
add_library(AtomicMem STATIC ${PHYSICS_SOURCES})

target_include_directories(AtomicMem PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)


# Test methods
add_executable(MemoryPoolTests tests/main.cpp)
target_link_libraries(MemoryPoolTests AtomicMem)


target_compile_definitions(AtomicMem PUBLIC
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)