#include "pch.h"
#include "hooks.h"

void Hooks::Init()
{
	const auto installForAssembly = [](const UR::Assembly* assembly)
	{
		if (!assembly) return;

		for (auto& factory : GetRegistry())
		{
			const auto hook = factory();
			if (hook->TargetAssembly() == assembly->name) hook->Install();
		}
	};

	if (UR::OnAssemblyLoaded(installForAssembly)) return;

	for (const auto& assembly : UR::assembly) installForAssembly(assembly.get());
}
