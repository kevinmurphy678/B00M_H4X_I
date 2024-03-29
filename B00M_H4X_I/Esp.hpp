#pragma once
#include<Chams.hpp>
namespace SourceEngine
{
	bool espE = false;
	bool esp_LOS = false;
	bool espSkeleton = false;

	float esp_CT_Color[] = { 55 / 255.f, 55 / 255.f, 255 / 255.f };
	float esp_T_Color[] = { 255 / 255.f, 255 / 255.f, 55 / 255.f };

	void DrawESP()
	{
		if(!espE) return;
		auto localPlayer = C_CSPlayer::GetLocalPlayer();

		for (int i = 1; i < Engine->GetMaxClients(); i++)
		{
			C_CSPlayer* player = static_cast<C_CSPlayer*>(EntitiyList->GetClientEntity(i));
			if (!player) continue;
			if (player == localPlayer) continue;
			if (!player->GetClientClass()->m_ClassID == EClassIds::CCSPlayer) continue; //Skip non players
			if (!player->IsAlive())continue;
			if (player->IsDormant())continue;
			player_info_t info;
			if (!Engine->GetPlayerInfo(i, &info))
				continue;
			Vector entityOrigin = player->GetOrigin();
			Vector entityOriginScreen;
			Vector head = Utils::GetEntityBone(player, ECSPlayerBones::head_0);
			head.z += 12.5f; //dunno why +12.5f
			Vector headScreen;
			if (Utils::WorldToScreen(entityOrigin, entityOriginScreen))
			{
				if (Utils::WorldToScreen(head, headScreen))
				{
					float h = abs(headScreen.y - entityOriginScreen.y)*0.95f;
					float w = h * 0.66f;
					Color hitBox;

					if (player->GetTeamNum() == localPlayer->GetTeamNum())
						hitBox = Color(esp_T_Color[0] * 255, esp_T_Color[1] * 255, esp_T_Color[2] * 255);
					else
						hitBox = Color(esp_CT_Color[0] * 255, esp_CT_Color[1] * 255, esp_CT_Color[2] * 255);

					DrawRect(entityOriginScreen.x - w / 2, entityOriginScreen.y - h, w, h, hitBox);

					wchar_t output[128];
					mbstowcs(output, info.szName, 128);
					std::wstring name = std::wstring(output);
				
					std::wstring health = L"HP: " + std::to_wstring(player->GetHealth());		

					Vector2D nameLength = GetTextSize(name.c_str(), font);
					Vector2D healthLength = GetTextSize(health.c_str(), font);
					
					DrawString(headScreen.x - nameLength.x / 2, headScreen.y, Color(255, 255, 255), name.c_str());
					DrawString(headScreen.x - healthLength.x / 2, headScreen.y + h, Color(255, 255, 255), health.c_str());

					if (esp_LOS)//Line of sight aim indicator
					{

						Vector src3D, dst3D, forward, src, dst;
						trace_t tr;
						Ray_t ray;
						CTraceFilter filter;
						forward = Vector();
						Utils::AngleVectors(player->GetEyeAngles(), forward);
						filter.pSkip = player;

						src3D = player->GetEyePos();
						dst3D = src3D + (forward * 8192);

						ray.Init(src3D, dst3D);

						EngineTrace->TraceRay(ray, MASK_SHOT, &filter, &tr);

						if (Utils::WorldToScreen(src3D, src) && Utils::WorldToScreen(tr.endpos, dst))
						{
							DrawLine(src.x, src.y, dst.x, dst.y, Color(255, 255, 255, 155));
							DrawRect(dst.x - 3, dst.y - 3, 6, 6, Color(255, 255, 255, 200));
						}

					
					}
					if (espSkeleton)
					{
						const studiohdr_t* pStudioModel = ModelInfo->FindModel(player->GetModel());
						if (!pStudioModel)
							return;

						static matrix3x4_t pBoneToWorldOut[128];
						if (player->SetupBones(pBoneToWorldOut, 128, 256, 0))
						{
							for (int i = 0; i < pStudioModel->numbones; i++)
							{
								mstudiobone_t* pBone = pStudioModel->pBone(i);
								if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
									continue;

								Vector vBonePos1;
								if (!Utils::WorldToScreen(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
									continue;

								Vector vBonePos2;
								if (!Utils::WorldToScreen(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
									continue;


								DrawLine(vBonePos1.x, vBonePos1.y, vBonePos2.x, vBonePos2.y, chamsE ? Color(0,0,0) : Color(255,255,255));
							}
						}

					}
			
					
				}

			}
		}
	}

}