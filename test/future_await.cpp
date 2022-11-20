#include <gtest/gtest.h>

#include "test_executor.h"
#include "future/future.h"
#include "future/future_await.h"

TEST(FutureAwait, StartCoroutine)
{
	using namespace cebreiro;

	constexpr size_t expected1 = 123123;
	constexpr size_t expected2 = 456456;
	constexpr size_t expected3 = 789789;

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
}

TEST(FutureAwait, CancelCoroutine)
{
	using namespace cebreiro;

	// arrange
	std::shared_ptr<TestExecutor> e = CreateExecutor();
	TestExecutor& executor = *e;

	std::atomic<bool> flag1 = false;
	std::atomic<bool> flag2 = false;

	bool execute1 = false;
	bool execute2 = false;

	// act
	auto coroutine = [&]() -> Future<void>
	{
		co_await Post([]()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));

			}).ConfigureAwait(executor);

		flag1.store(true);
		flag1.notify_one();

		std::this_thread::sleep_for(std::chrono::milliseconds(300));

		execute1 = true;

		flag2.store(true);
		flag2.notify_one();

		// awaiter will throw canceled exception
		co_await Post([]()
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			});

		execute2 = true;
	};

	Future<void> future = coroutine();
	flag1.wait(false);

	future.Cancel();
	flag2.wait(false);

	// assert
	ASSERT_ANY_THROW(future.Get());
	ASSERT_TRUE(execute1);
	ASSERT_FALSE(execute2);
}
//
//TEST(FutureAwait, UnhandledException)
//{
//	using namespace cebreiro;
//
//	// arrange
//
//	// act
//	auto coroutine = []() -> Future<int32_t>
//	{
//		std::cout << 123123123 << '\n';
//		throw std::runtime_error("exception");
//
//		co_return 123;
//	};
//
//	Future<int32_t> future = coroutine();
//
//	// assert
//	future.Get();
//
//	std::this_thread::sleep_for(std::chrono::seconds(1));
//}