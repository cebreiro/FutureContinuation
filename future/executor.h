#pragma once
#include <memory>
#include <functional>
#include "future/execute_options.h"

namespace cebreiro
{
	class Executor : public std::enable_shared_from_this<Executor>
	{
	public:
		virtual ~Executor();

		virtual bool IsRunningContext() const = 0;
		virtual void Post(const std::function<void()>& function, const ExecuteOption& option) = 0;

		static auto Current() -> Executor&;
		static auto GetDefault() -> Executor&;

		static void SetCurrent(Executor* executor);
		static void RemoveCurrent();
	};
}