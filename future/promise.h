#pragma once
#include "future/detail/execution_context.h"
#include "future/future.h"

namespace cebreiro
{
	template <typename T>
	class Promise
	{
	public:
		Promise(const Promise&) = delete;
		Promise(Promise&& rhs) noexcept = default;
		Promise& operator=(const Promise&) = delete;
		Promise& operator=(Promise&& rhs) noexcept = default;

		Promise();
		~Promise();

		void Set(T&& value);
		void Set(const T& value);
		void SetException(const std::exception_ptr& exception);

		auto GetFuture() -> Future<T>;

	private:
		ExecutionContext<T> _context;
	};

	template <>
	class Promise<void>
	{
	public:
		Promise(const Promise&) = delete;
		Promise(Promise&& rhs) noexcept = default;
		Promise& operator=(const Promise&) = delete;
		Promise& operator=(Promise&& rhs) noexcept = default;

		Promise();
		~Promise();

		void Set();
		void SetException(const std::exception_ptr& exception);

		auto GetFuture() -> Future<void>;

	private:
		ExecutionContext<void> _context;
	};

	template <typename T>
	Promise<T>::Promise()
		: _context(std::make_shared<detail::ExecutionContextImpl<T>>())
	{
	}

	template <typename T>
	Promise<T>::~Promise()
	{
	}

	template <typename T>
	void Promise<T>::Set(T&& value)
	{
		_context.Set(std::move(value));
	}

	template <typename T>
	void Promise<T>::Set(const T& value)
	{
		_context.Set(value);
	}

	template <typename T>
	void Promise<T>::SetException(const std::exception_ptr& exception)
	{
		_context.SetException(exception);
	}

	template <typename T>
	auto Promise<T>::GetFuture() -> Future<T>
	{
		return Future(_context);
	}
}
