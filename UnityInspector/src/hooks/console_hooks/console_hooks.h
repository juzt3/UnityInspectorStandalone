#pragma once
#include "hooks/hooks.h"

class ConsoleHooks : public IHook
{
public:
	std::string_view TargetAssembly() const override { return "UnityEngine.CoreModule.dll"; }
	void Install() override;

private:
	static void UNITY_CALLING_CONVENTION HDebugLogObject(void* message);
	static void UNITY_CALLING_CONVENTION HDebugLogString(void* message);
	static void UNITY_CALLING_CONVENTION HDebugLogFormat(void* message, void* args);
	static void UNITY_CALLING_CONVENTION HDebugLogWarningObject(void* message);
	static void UNITY_CALLING_CONVENTION HDebugLogWarningString(void* message);
	static void UNITY_CALLING_CONVENTION HDebugLogErrorObject(void* message);
	static void UNITY_CALLING_CONVENTION HDebugLogErrorString(void* message);
	static void UNITY_CALLING_CONVENTION HDebugLogException(void* exception);
	static void UNITY_CALLING_CONVENTION HDebugLogAssertion(void* message);
};
