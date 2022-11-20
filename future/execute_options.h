#pragma once
#include <cstdint>
#include <memory>

namespace cebreiro
{
	enum class ExecutePriority : int32_t
	{
		HIGH,
		ABOVE,
		NORMAL,
		BELOW,
		IDLE,
	};

	struct ExecuteOption
	{
		ExecutePriority policy = ExecutePriority::HIGH;
		int32_t group = -1;
	};
}