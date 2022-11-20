add_library(future STATIC)
target_include_directories(future PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_options(future PUBLIC
/std:c++20
/W4
/WX
)

target_sources(future PRIVATE
promise.cpp
executor.cpp
default_executor.cpp
)

add_subdirectory(detail)