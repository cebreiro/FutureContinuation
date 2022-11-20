#pragma once
#include "future/future_status.h"
#include <chrono>
#include <functional>
#include <exception>
#include <vector>
#include <mutex>

namespace cebreiro::detail
{
	class ExecutionContextImplBase
	{
	public:
		ExecutionContextImplBase();
		~ExecutionContextImplBase();

		void Cancel();

		auto Wait() -> FutureStatus;
		auto WaitFor(std::chrono::milliseconds milliseconds) -> FutureStatus;

		bool IsPending() const;
		bool IsDone() const;
		bool IsCanceled() const;

		auto Status() -> FutureStatus;

		void AddContinuation(const std::function<void()>& continuation);
		void SetException(const std::exception_ptr& exception);

	protected:
		bool Unsafe_IsPending() const;
		bool Unsafe_IsDone() const;
		bool Unsafe_IsCanceled() const;

	protected:
		mutable std::mutex _mutex;
		std::condition_variable _condVar;
		FutureStatus _status = FutureStatus::Pending;
		std::vector<std::function<void()>> _continuations;
		std::vector<std::exception_ptr> _exceptions;
	};

	template <typename T>
	class ExecutionContextImpl : public ExecutionContextImplBase
	{
	public:
		void Set(T&& value);
		void Set(const T& value);

		auto Get() -> decltype(auto);

	private:
		std::optional<T> _value = std::nullopt;
	};

	template <>
	class ExecutionContextImpl<void> : public ExecutionContextImplBase
	{
	public:
		void Set();
		void Get();
	};
}

#include "future/detail/execution_context_impl.ipp"