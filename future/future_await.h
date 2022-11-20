#pragma once
#include <coroutine>
#include "future/future.h"

namespace cebreiro
{
	template <typename T>
	struct FutureAwaiter
	{
		bool await_ready() const { return future.IsDone(); }
		void await_suspend(std::coroutine_handle<> handle)
		{
			future.ContinuationWith([handle]([[maybe_unused]] Future<T>& self)
				{
					handle.resume();

				}, Executor::Current(), option);
		}

		auto await_resume() -> decltype(auto)
		{
			return future.Get();
		}

		Future<T>& future;
		ExecuteOption option;
	};

	template <typename T>
	auto operator co_await(Future<T>&& future)
	{
		return FutureAwaiter<T>{
			.future = future,
			.option = ExecuteOption{},
		};
	}

	template <typename T>
	auto operator co_await(Future<T>& future)
	{
		return FutureAwaiter<T>{
			.future = future,
			.option = ExecuteOption{},
		};
	}

	template <typename T>
	struct FutureConfigureAwait
	{
		bool await_ready() const
		{
			if (executor.IsRunningContext())
			{
				return future.IsDone();
			}

			return false;
		}

		void await_suspend(std::coroutine_handle<> handle)
		{
			future.ContinuationWith([handle]([[maybe_unused]]Future<T>& self)
				{
					handle.resume();

				}, executor, option);
		}

		auto await_resume() -> decltype(auto)
		{
			return future.Get();
		}

		Executor& executor;
		Future<T>& future;
		ExecuteOption option;
	};

	template <typename T>
	auto Future<T>::ConfigureAwait(Executor& resumeExecutor) -> FutureConfigureAwait<T>
	{
		FutureConfigureAwait<T> awaiter{
			.executor = resumeExecutor,
			.future = *this,
			.option = ExecuteOption{},
		};
		return awaiter;
	}
}

namespace std
{
	template <typename T, typename... Args>
	struct coroutine_traits<cebreiro::Future<T>, Args...>
	{
		struct promise_type
		{
			promise_type()
				: context(std::make_shared<cebreiro::detail::ExecutionContextImpl<T>>())
			{
			}

			auto get_return_object() const
			{
				return cebreiro::Future<T>(context);
			}
			auto initial_suspend() const noexcept { return std::suspend_never{}; }
			auto final_suspend() const noexcept { return std::suspend_never{}; }
			void return_value(T&& value) { context.Set(std::move(value)); }
			void return_value(const T& value) { context.Set(value); }
			void unhandled_exception()
			{
				context.SetException(std::current_exception());
			}

		private:
			cebreiro::ExecutionContext<T> context;
		};
	};

	template <typename... Args>
	struct coroutine_traits<cebreiro::Future<void>, Args...>
	{
		struct promise_type
		{
			promise_type()
				: context(std::make_shared<cebreiro::detail::ExecutionContextImpl<void>>())
			{
			}

			auto get_return_object() const { return cebreiro::Future<void>(context); }
			auto initial_suspend() const noexcept { return std::suspend_never{}; }
			auto final_suspend() const noexcept { return std::suspend_never{}; }
			void return_void() { context.Set(); }
			void unhandled_exception() { context.SetException(std::current_exception()); }

		private:
			cebreiro::ExecutionContext<void> context;
		};
	};
}
