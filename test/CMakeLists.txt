add_executable(future_test
main.cpp
future_creation.cpp
test_executor.cpp
future_then.cpp
future_exception.cpp
future_cancel.cpp
future_await.cpp
)

include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG        v1.12.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(future_test 
future
GTest::gtest 
GTest::gtest_main
)