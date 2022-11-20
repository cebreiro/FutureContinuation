#include <gtest/gtest.h>

#include "test_executor.h"
#include "future/future.h"

TEST(FutureThen, VoidParam)
{
	using namespace cebreiro;

	constexpr size_t expected1 = 123123;
	constexpr size_t expected2 = 36345345;

	// arrange
	std::shared_ptr<TestExecutor> e1 = CreateExecutor();
	std::shared_ptr<TestExecutor> e2 = CreateExecutor();

	TestExecutor& executor1 = *e1;
	TestExecutor& executor2 = *e2;

	// act

	std::thread::id thread1;
	Future<std::unique_ptr<size_t>> f1 = StartAsync([&]()
		{
			thread1 = std::this_thread::get_id();
			return std::make_unique<size_t>(expected1);
		}, executor1);

	std::thread::id thread2;
	Future<size_t> f2 = f1.Then([&]()
		{
			thread2 = std::this_thread::get_id();
			return expected2;
		}, executor2);


	std::unique_ptr<size_t> result1 = f1.Get();
	size_t result2 = f2.Get();

	// assert
	ASSERT_TRUE(result1.operator bool());
	ASSERT_EQ(*result1, expected1);
	ASSERT_EQ(result2, expected2);
	ASSERT_EQ(thread1, executor1.GetId());
	ASSERT_EQ(thread2, executor2.GetId());
}

TEST(FutureThen, SingleParam)
{
	using namespace cebreiro;

	constexpr size_t expected1 = 123123;

	// arrange
	std::shared_ptr<TestExecutor> e = CreateExecutor();
	TestExecutor& executor = *e;

	// act
	Future<std::unique_ptr<size_t>> f1 = StartAsync([]()
		{
			return std::make_unique<size_t>(expected1);
		}, executor);

	std::atomic<size_t> result1 = 0;
	Future<void> f2 = f1.Then([&result1](std::unique_ptr<size_t> param)
		{
			result1 = *param;
		}, executor);
	(void)f2.Wait();

	// assert
	ASSERT_FALSE(f1.Get().operator bool());
	ASSERT_EQ(result1, expected1);
}