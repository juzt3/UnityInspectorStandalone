#include "pch.h"
#include "features.h"
#include "lua_system/lua_system.h"

void IFeature::Init()
{
}

void IFeature::Render()
{
}

namespace Features
{
	static std::vector<std::unique_ptr<IFeature>> features;
	static std::atomic_bool initialized = false;
	static std::mutex featureMutex;

	void Init()
	{
		for (auto& factory : GetRegistry())
			features.push_back(factory());

		features.push_back(std::make_unique<LuaSystem>());

		for (const auto& feature : features)
			feature->Init();

		initialized = true;
	}

	void Update(float deltaTime)
	{
		if (!initialized) return;
		const std::unique_lock lock(featureMutex, std::try_to_lock);
		if (!lock.owns_lock()) return;
		for (const auto& feature : features)
			feature->Update(deltaTime);
	}

	void Render()
	{
		if (!initialized) return;
		const std::scoped_lock lock(featureMutex);
		for (const auto& feature : features)
			feature->Render();
	}

}
