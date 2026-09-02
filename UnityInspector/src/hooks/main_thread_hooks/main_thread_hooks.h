#pragma once
#include "hooks/hooks.h"

class MainThreadHooks final : public IHook
{
public:
	std::string_view TargetAssembly() const override { return "UnityEngine.CoreModule.dll"; }
	void Install() override;

private:
	static void UNITY_CALLING_CONVENTION HExec(void* instance, void* methodInfo);
};
