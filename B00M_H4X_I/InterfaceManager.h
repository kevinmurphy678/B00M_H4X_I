#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include "SourceEngine\SDK.hpp"
namespace SourceEngine
{

	static IBaseClientDLL* Client = nullptr;
	static IPanel* Panel = nullptr;;
	static ISurface* Surface = nullptr;
	static IClientMode* ClientMode = nullptr;
	static IVEngineClient* Engine = nullptr;
	static IClientEntityList* EntitiyList = nullptr;
	static IEngineTrace* EngineTrace = nullptr;
	static CGlobalVarsBase* GlobalVars = nullptr;
	static IInputSystem* Input = nullptr;
	static IVModelRender* ModelRender = nullptr;
	static IVModelInfo* ModelInfo = nullptr;
	static IMaterialSystem* Material = nullptr;

	using tCreateInterface = void*(*)(const char*, int*);

	class InterfaceManager
	{
	public:
		InterfaceManager() = default;
		~InterfaceManager() = default;

		template <typename T>
		static T* Grab(const std::string& moduleName, const std::string& interfaceName);

		template <typename T>
		static T* BruteForce(const std::string& moduleName, const std::string& toBruteForce);
	};

	template <typename T>
	T* InterfaceManager::Grab(const std::string& moduleName, const std::string& interfaceName)
	{
		auto factory = reinterpret_cast<tCreateInterface>(GetProcAddress(GetModuleHandle(moduleName.c_str()), "CreateInterface"));
		if (factory)
			return (T*)factory(interfaceName.c_str(), nullptr);

		return nullptr;
	}

	template <typename T>
	T* InterfaceManager::BruteForce(const std::string& moduleName, const std::string& toBruteForce)
	{
		auto factory = reinterpret_cast<tCreateInterface>(GetProcAddress(GetModuleHandle(moduleName.c_str()), "CreateInterface"));
		if (!factory) return nullptr;

		char bruteforce[256];
		for (auto i = 0; i < 100; i++)
		{
			sprintf_s(bruteforce, sizeof(bruteforce), "%s0%i", toBruteForce.c_str(), i);

			if (factory(bruteforce, nullptr))
				return (T*)factory(bruteforce, nullptr);

			sprintf_s(bruteforce, sizeof(bruteforce), "%s00%i", toBruteForce.c_str(), i);

			if (factory(bruteforce, nullptr))
				return (T*)factory(bruteforce, nullptr);
		}

		return nullptr;
	}


}