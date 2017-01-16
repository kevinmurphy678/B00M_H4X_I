#pragma once

namespace SourceEngine
{


	typedef unsigned short MaterialHandle_t;

	class IMaterialSystem
	{
	public:
		IMaterial* CreateMaterial(const char* pMaterialName, KeyValues* pVMTKeyValues)
		{
			typedef IMaterial* (__thiscall *oCreateMaterial)(void*, const char*, KeyValues*);
			return CallVFunction<oCreateMaterial>(this, 83)(this, pMaterialName, pVMTKeyValues);
		}

		IMaterial* FindMaterial(char const* pMaterialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = NULL)
		{
			typedef IMaterial* (__thiscall *oFindMaterial)(void*, char const*, const char*, bool, const char*);
			return CallVFunction<oFindMaterial>(this, 84)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
		}

		MaterialHandle_t FirstMaterial()
		{
			typedef MaterialHandle_t(*oFirstMaterial)(void*);
			return CallVFunction<oFirstMaterial>(this, 86)(this);
		}

		MaterialHandle_t NextMaterial(MaterialHandle_t h)
		{
			typedef MaterialHandle_t(*oNextMaterial)(void*, MaterialHandle_t);
			return CallVFunction<oNextMaterial>(this, 87)(this, h);
		}

		MaterialHandle_t InvalidMaterial()
		{
			typedef MaterialHandle_t(*oInvalidMaterial)(void*);
			return CallVFunction<oInvalidMaterial>(this, 88)(this);
		}

		IMaterial* GetMaterial(MaterialHandle_t h)
		{
			typedef IMaterial* (*oGetMaterial)(void*, MaterialHandle_t);
			return CallVFunction<oGetMaterial>(this, 89)(this, h);
		}
	};
}