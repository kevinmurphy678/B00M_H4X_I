#pragma once

#include "SourceEngine\SDK.hpp"
using SourceEngine::Vector;
namespace SourceEngine
{
	

	class IVModelInfo {
	public:
		inline void* GetModel(int Index) {
			return SourceEngine::CallVFunction<void*(__thiscall *)(void*, int)>(this, 1)(this, Index);
		}

		inline int GetModelIndex(const char* Filename) {
			return SourceEngine::CallVFunction<int(__thiscall *)(void*, const char*)>(this, 2)(this, Filename);
		}

		inline const char* GetModelName(const void* Model) {
			return SourceEngine::CallVFunction<const char* (__thiscall *)(void*, const void*)>(this, 3)(this, Model);
		}

		inline const studiohdr_t *FindModel(const void* Model) {
			return SourceEngine::CallVFunction<studiohdr_t*(__thiscall *)(void*, const void*)>(this, 30)(this, Model);
		}
	};
}