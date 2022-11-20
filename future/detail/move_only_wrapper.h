#pragma once

namespace cebreiro
{
	namespace detail
	{
		template <typename Callable>
		auto Wrap(Callable&& callable)
		{
			if constexpr (std::is_copy_constructible_v<std::decay_t<Callable>>)
			{
				return callable;
			}
			else
			{
				auto shared = std::make_shared<std::decay_t<Callable>>(std::forward<Callable>(callable));
				return [shared] <typename... Ts> (Ts&&... args) 
				{
					return (*shared)(std::forward<Ts>(args)...);
				};
			}
		}
	}
}