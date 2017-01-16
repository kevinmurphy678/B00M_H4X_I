#pragma once
#include<Hookers.h>
#include<Misc.hpp>
namespace SourceEngine
{
	//Config
	float aimbotFov = 15.f;

	//Toggles
	bool aimbotE = false;
	bool aimbotAutoShoot = false;

	//Is visible
	bool IsVisible(IClientEntity* pEntity, ECSPlayerBones bone)
	{
		Vector e_vecHead = Utils::GetEntityBone(pEntity, bone);
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		Vector p_vecHead = pLocal->GetEyePos();

		Ray_t ray;
		trace_t tr;
		ray.Init(p_vecHead, e_vecHead);
		CTraceFilter traceFilter;
		traceFilter.pSkip = pLocal;
		EngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

		return tr.m_pEnt == pEntity;
	}
	//closest enemy
	C_CSPlayer* GetClosestEnemy(CUserCmd* pCmd)
	{
		C_CSPlayer* closestEntity = NULL;
		auto pLocal = C_CSPlayer::GetLocalPlayer();
		float dist = 10000000.0f;

		for (int i = 1; i < Engine->GetMaxClients(); i++)
		{
			C_CSPlayer* player = static_cast<C_CSPlayer*>(EntitiyList->GetClientEntity(i));
			if (!player) continue;
			if (!player->GetClientClass()->m_ClassID == EClassIds::CCSPlayer) continue; //Skip non players
			if (!player->IsAlive())continue;
			if (player->IsDormant())continue;
			if (player->GetTeamNum() == pLocal->GetTeamNum())continue;

			if (!IsVisible(player, ECSPlayerBones::head_0)) continue;


			float distPlayer = pLocal->GetOrigin().DistToSqr(player->GetOrigin());
			float fov = Utils::GetFOV(pCmd->viewangles, pLocal->GetEyePos(), Utils::GetEntityBone(player, head_0));
			if (distPlayer < dist && fov < aimbotFov)
			{
				closestEntity = player;
				dist = distPlayer;
			}
		}

		return closestEntity;
	}

	bool __stdcall CreateMove_Aimbot(float sample_input_frametime, CUserCmd* pCmd)
	{
		if (!aimbotE) return false;

		C_CSPlayer* localPlayer = static_cast<C_CSPlayer*>(C_CSPlayer::GetLocalPlayer());
		if (!localPlayer) return false;
	

		C_CSPlayer* player = GetClosestEnemy(pCmd);
		
		if (!player) return false;

		Vector start, end;

		end = Utils::GetEntityBone(player, ECSPlayerBones::head_0);
		start = localPlayer->GetEyePos();

		Vector dir = end - start;
		Vector newAngles = Vector();
		
		dir = dir.Normalized();

		Utils::VectorAngles(dir, newAngles);

		auto punchAngles = *localPlayer->AimPunch() * 2.0f;
		newAngles -= punchAngles;

		if (aimbotAutoShoot)
		{
			C_BaseCombatWeapon* weapon = localPlayer->GetActiveWeapon();
			if (weapon && !weapon->isKnife() && weapon->GetAmmo() > 0)
			{
				float nextPrimaryAttack = weapon->NextPrimaryAttack();
				float tick = localPlayer->GetTickBase() * GlobalVars->interval_per_tick;

				if (nextPrimaryAttack >= tick)
				{
					pCmd->buttons &= ~IN_ATTACK;
				}
				else
				{
					pCmd->buttons |= IN_ATTACK;
				}
			}
		}

		if (!Utils::Clamp(newAngles))
		{
			abort();

		}

		//auto punchAngles = *pLocal->AimPunch() * 2.0f;
		//if (punchAngles.x != 0.0f || punchAngles.y != 0) {

		//	newAngles.x += oldx;
		//	newAngles.y += oldy;

		//	newAngles -= punchAngles;
		//	if (!Utils::Clamp(pCmd->viewangles)) {
		//		abort();
		//	}

		//	oldx = punchAngles.x;
		//	oldy = punchAngles.y;
		//}

		pCmd->viewangles = newAngles;
	}

	void __stdcall PaintTraverse_Aimbot(unsigned int panel, bool forceRepaint, bool allowForce)
	{
		if (!aimbotE) return;

		int iScreenWidth, iScreenHeight;
		Engine->GetScreenSize(iScreenWidth, iScreenHeight);

		float radius = aimbotFov / misc_Fov * iScreenWidth / 2.f; 

		DrawCircle((iScreenWidth / 2), (iScreenHeight / 2), radius, Color(255, 0, 00)); //Fov crosshair		

		//DrawRect(iScreenWidth / 2 - 8, iScreenHeight / 2 - 8, 16, 16, Color(255, 0, 0, 155));
	}
}