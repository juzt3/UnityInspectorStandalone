#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <safetyhook.hpp>

class HookManager
{
public:
	template <typename Fn>
	    requires std::is_pointer_v<Fn> && std::is_function_v<std::remove_pointer_t<Fn>>
	static auto Install(Fn func, Fn handler) -> bool
	{
		const auto target = reinterpret_cast<void*>(func);
		const auto destination = reinterpret_cast<void*>(handler);
		if (!target || !destination) return false;

		std::unique_lock guard(lock);
		if (hooks.contains(destination)) return false;
		for (const auto& [_, hook] : hooks)
		{
			if (hook->target() == target) return false;
		}

		auto hook = SafetyHookInline::create(target, destination);
		if (!hook) return false;

		hooks.emplace(destination, std::make_shared<SafetyHookInline>(std::move(*hook)));
		return true;
	}

	template <typename Fn>
	    requires std::is_pointer_v<Fn> && std::is_function_v<std::remove_pointer_t<Fn>>
	static auto Detach(Fn handler) noexcept -> void
	{
		std::unique_lock guard(lock);
		hooks.erase(reinterpret_cast<void*>(handler));
	}

	template <typename RType, typename... Params>
	static auto Call(RType (*handler)(Params...), Params... params) -> RType
	{
		return Invoke(handler, std::forward<Params>(params)...);
	}

	template <typename RType, typename... Params>
	static auto Ccall(RType(__cdecl* handler)(Params...), Params... params) -> RType
	{
		return Invoke(handler, std::forward<Params>(params)...);
	}

	template <typename RType, typename... Params>
	static auto Scall(RType(__stdcall* handler)(Params...), Params... params) -> RType
	{
		return Invoke(handler, std::forward<Params>(params)...);
	}

	template <typename RType, typename... Params>
	static auto Fcall(RType(__fastcall* handler)(Params...), Params... params) -> RType
	{
		return Invoke(handler, std::forward<Params>(params)...);
	}

	template <typename RType, typename... Params>
	static auto Vcall(RType(__vectorcall* handler)(Params...), Params... params) -> RType
	{
		return Invoke(handler, std::forward<Params>(params)...);
	}

	static auto DetachAll() noexcept -> void
	{
		std::unique_lock guard(lock);
		hooks.clear();
	}

private:
	using HookPtr = std::shared_ptr<SafetyHookInline>;

	static auto GetHook(void* handler) noexcept -> HookPtr
	{
		std::shared_lock guard(lock);
		const auto entry = hooks.find(handler);
		return entry != hooks.end() ? entry->second : nullptr;
	}

	template <typename Fn, typename... Args>
	static auto Invoke(Fn handler, Args&&... args) -> std::invoke_result_t<Fn, Args...>
	{
		using Result = std::invoke_result_t<Fn, Args...>;
		const auto hook = GetHook(reinterpret_cast<void*>(handler));
		if (hook)
		{
			return std::invoke(hook->original<Fn>(), std::forward<Args>(args)...);
		}

		if constexpr (!std::is_void_v<Result>) return Result{};
	}

	inline static std::shared_mutex lock{};
	inline static std::unordered_map<void*, HookPtr> hooks{};
};
