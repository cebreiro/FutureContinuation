# FutureContinuation
## Overview
because C++ has no 'executor', std::future has no 'then' method.

because std::future has no 'then' method, Despite the presence of coroutine, C++ has no async-await pattern.

this repository will show you awaitable future object by defining 'custom' executor.

## Quick Exmaple
```cpp
// arrange
std::shared_ptr<TestExecutor> e = CreateExecutor();
TestExecutor& executor = *e;

std::thread::id thread1;
std::thread::id thread2;
std::thread::id thread3;

size_t result1 = 0;
size_t result2 = 0;
size_t result3 = 0;

// act
// note: the capturing lambda is not safe
// see C++ core guidelines
auto coroutine = [&]() -> Future<int32_t>
{
	thread1 = std::this_thread::get_id();

	result1 = co_await Post([]()
		{
			return expected1;

		}).ConfigureAwait(executor);

	thread2 = std::this_thread::get_id();

	result2 = co_await Post([]()
		{
			return expected2;
		});

	thread3 = std::this_thread::get_id();

	co_return expected3;
};

result3 = coroutine().Get();

// assert
ASSERT_EQ(thread1, std::this_thread::get_id());
ASSERT_EQ(thread2, executor.GetId());
ASSERT_EQ(thread3, executor.GetId());

ASSERT_EQ(result1, expected1);
ASSERT_EQ(result2, expected2);
ASSERT_EQ(result3, expected3);
```

## Build
### CMake
```cmake
cmake -B ./out/build -S ./
cmake --build ./out/build
```
gtest library dependency will be automatically managed by CMake.
