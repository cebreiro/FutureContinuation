#pragma once
#include "future/detail/execution_context_impl.h"
#include "future/detail/callable_traits.h"
#include "future/executor.h"

namespace cebreiro
{
	template <typename T>
	class ExecutionContext
	{
	public:
		ExecutionContext() = default;
		ExecutionContext(const ExecutionContext&) = default;
		ExecutionContext(ExecutionContext&& rhs) noexcept = default;
		ExecutionContext& operator=(const ExecutionContext&) = default;
		ExecutionContext& operator=(ExecutionContext&& rhs) noexcept = default;

		explicit ExecutionContext(std::shared_ptr<detail::ExecutionContextImpl<T>> impl);
		~ExecutionContext();

		void Cancel();
		void Set(T&& value);
		void Set(const T& value);

		void AddContinuation(const std::function<void()>& continuation);
		void SetException(const std::exception_ptr& exception);

		auto Get() -> decltype(auto);

		auto Wait() -> FutureStatus;
		auto WaitFor(std::chrono::milliseconds milliseconds) -> FutureStatus;

		bool IsPending() const;
		bool IsDone() const;
		bool IsCanceled() const;
		auto Status() -> FutureStatus;

	private:
		std::shared_ptr<detail::ExecutionContextImpl<T>> _impl;
	};

	template <>
	class ExecutionContext<void>
	{
	public:
		ExecutionContext() = default;
		ExecutionContext(const ExecutionContext&) = default;
		ExecutionContext(ExecutionContext&& rhs) noexcept = default;
		ExecutionContext& operator=(const ExecutionContext&) = default;
		ExecutionContext& operator=(ExecutionContext&& rhs) noexcept = default;

		explicit ExecutionContext(std::shared_ptr<detail::ExecutionContextImpl<void>> impl);
		~ExecutionContext();

		void Cancel();
		void Set();

		void AddContinuation(const std::function<void()>& continuation);
		void SetException(const std::exception_ptr& exception);

		void Get();

		auto Wait() -> FutureStatus;
		auto WaitFor(std::chrono::milliseconds milliseconds) -> FutureStatus;

		bool IsPending() const;
		bool IsDone() const;
		bool IsCanceled() const;
		auto Status() -> FutureStatus;

	private:
		std::shared_ptr<detail::ExecutionContextImpl<void>> _impl;
	};
}

#include "future/detail/execution_context.ipp"