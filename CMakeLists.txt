cmake_minimum_required(VERSION 3.10)

project(watersimulation)

# ── C++ Standard ──────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Compiler Flags ────────────────────────────────────────────────
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math")

# # ── OpenMP ────────────────────────────────────────────────────────
# find_package(OpenMP REQUIRED)

# ── Main Executable ───────────────────────────────────────────────
add_executable(watersimulation
    src/main.cpp
    src/renderer.cpp
    src/spatialhashgrid.cpp
    src/renderer_util.cpp
    src/sph.cpp
    src/solver.cpp
    src/random.cpp
    src/vec2.cpp
    src/particle.cpp
    src/fps.cpp
    src/timer.cpp
    src/thread_pool.cpp

)

target_include_directories(watersimulation PRIVATE include)
# target_link_libraries(watersimulation SDL2 SDL2main OpenMP::OpenMP_CXX)
target_link_libraries(watersimulation SDL2 SDL2main )
