#pragma once

#include <Windows.h>
#include "SourceEngine/SDK.hpp"
#include "NetvarManager.hpp"
#include "XorStr.hpp"
#include "SourceEngine\Definitions.hpp"

class C_CSPlayer;
namespace SourceEngine {

	class IPlayerInfoManager
	{
	public:
		CGlobalVarsBase* GetGlobalVars()
		{
			typedef CGlobalVarsBase*(__thiscall* GetGlobalVarsFn)(void*);
			return SourceEngine::CallVFunction<GetGlobalVarsFn>(this, 1)(this);
		}
	};

	class C_BaseCombatWeapon : public SourceEngine::IClientEntity
	{
		template<class T>
		inline T GetFieldValue(int offset)
		{
			return *(T*)((DWORD)this + offset);
		}

	public:

		C_CSPlayer* GetOwner()
		{
			using namespace SourceEngine;
			static int m_hOwnerEntity = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_hOwnerEntity"));
			return (C_CSPlayer*)Interfaces::EntityList()->GetClientEntityFromHandle(GetFieldValue<CHandle<C_CSPlayer>>(m_hOwnerEntity));
		}
		float NextPrimaryAttack()
		{

			static int m_flNextPrimaryAttack = GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("LocalActiveWeaponData"), XorStr("m_flNextPrimaryAttack"));
			return GetFieldValue<float>(m_flNextPrimaryAttack);
		}
		int GetId()
		{
			typedef int(__thiscall* tGetId)(void*);
			return SourceEngine::CallVFunction<tGetId>(this, 458)(this);
		}
		const char* GetName()
		{
			typedef const char* (__thiscall* tGetName)(void*);
			return SourceEngine::CallVFunction<tGetName>(this, 378)(this);
		}
		int GetItemDefinitionIndex()
		{
			static int m_iItemDefinitionIndex = GET_NETVAR(XorStr("DT_BaseAttributableItem"), XorStr("m_AttributeManager"), XorStr("m_Item"), XorStr("m_iItemDefinitionIndex"));
			return GetFieldValue<int>(m_iItemDefinitionIndex);
		}


		int GetAmmo()
		{

			static int m_iClip1 = GET_NETVAR(XorStr("DT_BaseCombatWeapon"), XorStr("m_iClip1"));
			return GetFieldValue<int>(m_iClip1);
		}

		bool isPistol()
		{

			switch (GetItemDefinitionIndex())
			{
			case WEAPON_DEAGLE:
			case WEAPON_ELITE:
			case WEAPON_FIVESEVEN:
			case WEAPON_GLOCK:
			case WEAPON_TEC9:
			case WEAPON_HKP2000:
			case WEAPON_USP_SILENCER:
			case WEAPON_P250:
			case WEAPON_CZ75A:
			case WEAPON_REVOLVER:
				return true;
			default:
				return false;
			}
		}

		bool isAutomatic()
		{
			switch (GetItemDefinitionIndex())
			{
			case WEAPON_AK47:
			case WEAPON_AUG:
			case WEAPON_FAMAS:
			case WEAPON_GALILAR:
			case WEAPON_M249:
			case WEAPON_M4A1:
			case WEAPON_M4A1_SILENCER:
			case WEAPON_MAC10:
			case WEAPON_P90:
			case WEAPON_UMP45:
			case WEAPON_BIZON:
			case WEAPON_NEGEV:
			case WEAPON_MP7:
			case WEAPON_MP9:
			case WEAPON_SG556:
				return true;
			default:
				return false;
			}
		}

		bool isKnife()
		{
			switch (GetItemDefinitionIndex())
			{
			case WEAPON_KNIFE:
			case WEAPON_KNIFE_T:
			case WEAPON_KNIFE_GUT:
			case WEAPON_KNIFE_FLIP:
			case WEAPON_KNIFE_BAYONET:
			case WEAPON_KNIFE_M9_BAYONET:
			case WEAPON_KNIFE_KARAMBIT:
			case WEAPON_KNIFE_TACTICAL:
			case WEAPON_KNIFE_BUTTERFLY:
			case WEAPON_KNIFE_SURVIVAL_BOWIE:
			case WEAPON_KNIFE_FALCHION:
			case WEAPON_KNIFE_PUSH:
				return true;
			default:
				return false;
			}
		}

		bool isGrenade()
		{
			switch (GetItemDefinitionIndex())
			{
			case WEAPON_SMOKEGRENADE:
			case WEAPON_HEGRENADE:
			case WEAPON_INCGRENADE:
			case WEAPON_FLASHBANG:
			case WEAPON_MOLOTOV:
			case WEAPON_DECOY:
				return true;
			default:
				return false;
			}
		}

		bool CanScope()
		{
			switch (GetItemDefinitionIndex())
			{
			case WEAPON_AUG:
			case WEAPON_AWP:
			case WEAPON_G3SG1:
			case WEAPON_SCAR20:
			case WEAPON_SG556:
			case WEAPON_SSG08:
				return true;
			default:
				return false;
			}
		}
	};


	class C_CSPlayer : public SourceEngine::IClientEntity
	{
		template<class T>
		inline T GetFieldValue(int offset)
		{
			return *(T*)((DWORD)this + offset);
		}

		template<class T>
		T* GetFieldPointer(int offset)
		{
			return (T*)((DWORD)this + offset);
		}
	public:
		static C_CSPlayer* GetLocalPlayer()
		{
			return (C_CSPlayer*)SourceEngine::Interfaces::EntityList()->GetClientEntity(SourceEngine::Interfaces::Engine()->GetLocalPlayer());
		}
		C_BaseCombatWeapon* GetActiveWeapon()
		{
			using namespace SourceEngine;
			static int m_hActiveWeapon = GET_NETVAR(XorStr("DT_BaseCombatCharacter"), XorStr("m_hActiveWeapon"));
			return (C_BaseCombatWeapon*)Interfaces::EntityList()->GetClientEntityFromHandle(GetFieldValue<CHandle<IClientEntity>>(m_hActiveWeapon));
		}
		int GetHealth()
		{
			static int m_iHealth = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_iHealth"));
			return GetFieldValue<int>(m_iHealth);
		}
		int GetTickBase()
		{
			static int m_nTickBase = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_nTickBase"));
			return GetFieldValue<int>(m_nTickBase);
		}
		bool IsAlive()
		{
			static int m_lifeState = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_lifeState"));
			return GetFieldValue<int>(m_lifeState) == 0;
		}
		int GetTeamNum()
		{
			static int m_iTeamNum = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_iTeamNum"));
			return GetFieldValue<int>(m_iTeamNum);
		}
		int GetFlags()
		{
			static int m_fFlags = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("m_fFlags"));
			return GetFieldValue<int>(m_fFlags);
		}
		float* GetMaxAlpha()
		{
			static int alpha = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_flFlashMaxAlpha"));
			return GetFieldPointer<float>(alpha);
		}
		SourceEngine::Vector GetViewOffset()
		{
			static int m_vecViewOffset = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_vecViewOffset[0]"));
			return GetFieldValue<SourceEngine::Vector>(m_vecViewOffset);
		}
		SourceEngine::Vector GetOrigin()
		{
			static int m_vecOrigin = GET_NETVAR(XorStr("DT_BaseEntity"), XorStr("m_vecOrigin"));
			return GetFieldValue<SourceEngine::Vector>(m_vecOrigin);
		}
		SourceEngine::Vector GetEyePos()
		{
			return GetOrigin() + GetViewOffset();
		}
		SourceEngine::Vector GetEyeAngles()
		{
			static int angles = GET_NETVAR(XorStr("DT_CSPlayer"), XorStr("m_angEyeAngles[0]"));
			return GetFieldValue<SourceEngine::Vector>(angles);
		}



		SourceEngine::Vector* ViewPunch()
		{
			static int m_viewPunchAngle = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_viewPunchAngle"));
			return GetFieldPointer<SourceEngine::Vector>(m_viewPunchAngle);
		}
		SourceEngine::Vector* AimPunch()
		{
			static int m_aimPunchAngle = GET_NETVAR(XorStr("DT_BasePlayer"), XorStr("localdata"), XorStr("m_Local"), XorStr("m_aimPunchAngle"));
			return GetFieldPointer<SourceEngine::Vector>(m_aimPunchAngle);
		}
	};
}