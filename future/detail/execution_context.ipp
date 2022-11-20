#pragma once

namespace cebreiro
{
	template <typename T>
	ExecutionContext<T>::ExecutionContext(std::shared_ptr<detail::ExecutionContextImpl<T>> impl)
		: _impl(std::move(impl))
	{
	}

	template <typename T>
	ExecutionContext<T>::~ExecutionContext()
	{
	}

	template <typename T>
	void ExecutionContext<T>::Cancel()
	{
		_impl->Cancel();
	}

	template <typename T>
	void ExecutionContext<T>::Set(T&& value)
	{
		_impl->Set(std::move(value));
	}

	template <typename T>
	void ExecutionContext<T>::Set(const T& value)
	{
		_impl->Set(value);
	}

	template <typename T>
	void ExecutionContext<T>::AddContinuation(const std::function<void()>& continuation)
	{
		_impl->AddContinuation(continuation);
	}

	template <typename T>
	void ExecutionContext<T>::SetException(const std::exception_ptr& exception)
	{
		_impl->SetException(exception);
	}

	template <typename T>
	auto ExecutionContext<T>::Get() -> decltype(auto)
	{
		return _impl->Get();
	}

	template <typename T>
	auto ExecutionContext<T>::Wait() -> FutureStatus
	{
		return _impl->Wait();
	}

	template <typename T>
	auto ExecutionContext<T>::WaitFor(std::chrono::milliseconds milliseconds) -> FutureStatus
	{
		return _impl->WaitFor(milliseconds);
	}

	template <typename T>
	bool ExecutionContext<T>::IsPending() const
	{
		return _impl->IsPending();
	}

	template <typename T>
	bool ExecutionContext<T>::IsDone() const
	{
		return _impl->IsDone();
	}

	template <typename T>
	bool ExecutionContext<T>::IsCanceled() const
	{
		return _impl->IsCanceled();
	}

	template <typename T>
	auto ExecutionContext<T>::Status() -> FutureStatus
	{
		return _impl->Status();
	}
}