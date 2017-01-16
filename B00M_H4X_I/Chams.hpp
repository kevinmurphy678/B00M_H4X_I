#pragma once

#include<Hookers.h>
namespace SourceEngine
{
	static IMaterial* materialChams;
	static IMaterial* materialChamsIgnorez;
	static IMaterial* materialChamsFlat;
	static IMaterial* materialChamsFlatIgnorez;
	static IMaterial* materialChamsArms;
	#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin)

	static bool chamsE = false;
	static bool chamsNoHands = false;
	static bool chamsWireFrameHands = false;
	float chamsAllyColor[] = { 5/255.f,225 / 255.f,255 / 255.f };
	float chamsAllyColor_Visible[] = { 5 / 255.f,89 / 255.f,255 / 255.f };

	float chamsEnemyColor[] = { 255 / 255.f,212 / 255.f,5 / 255.f };
	float chamsEnemyColor_Visible[] = { 255 / 255.f,42 / 255.f,5 / 255.f };

	float chamsHands[] = { 255 / 255.f,255 / 255.f, 255 / 255.f };

	static int created = 0;
	static IMaterial* CreateMaterial(std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, bool halflambert)
	{
		std::stringstream materialData;
		materialData << "\"" + type + "\"\n"
			"{\n"
			"\t\"$basetexture\" \"" + texture + "\"\n"
			"\t\"$ignorez\" \"" + std::to_string(ignorez) + "\"\n"
			"\t\"$nofog\" \"" + std::to_string(nofog) + "\"\n"
			"\t\"$model\" \"" + std::to_string(model) + "\"\n"
			"\t\"$nocull\" \"" + std::to_string(nocull) + "\"\n"
			"\t\"$halflambert\" \"" + std::to_string(halflambert) + "\"\n"
			"}\n" << std::flush;

		std::string materialName = "getRekt_" + std::to_string(RandomInt(10, 100000));
		KeyValues* keyValues = new KeyValues(materialName.c_str());

		InitKeyValues(keyValues, type.c_str());
		LoadFromBuffer(keyValues, materialName.c_str(), materialData.str().c_str(), nullptr, NULL, nullptr);

		return Material->CreateMaterial(materialName.c_str(), keyValues);
	}
	void __stdcall Chams_DrawModelExecute(IMatRenderContext* ctx,const DrawModelState_t &state,const ModelRenderInfo_t &pInfo,matrix3x4_t *pCustomBoneToWorld)
	{
		if (!chamsE) return;

		static bool created = false;
		if (!created)
		{
			materialChams = CreateMaterial("VertexLitGeneric", "VGUI/white_additive", false, true, true, true, true);
			materialChamsIgnorez = CreateMaterial("VertexLitGeneric", "VGUI/white_additive", true, true, true, true, true);
			materialChamsFlat = CreateMaterial("UnlitGeneric", "VGUI/white_additive", false, true, true, true, true);
			materialChamsFlatIgnorez = CreateMaterial("UnlitGeneric", "VGUI/white_additive", true, true, true, true, true);
			materialChamsArms = CreateMaterial("VertexLitGeneric", "VGUI/white_additive", false, true, true, true, true);
			created = true;
		}

		if (pInfo.pModel)
		{
			std::string modelName = ModelInfo->GetModelName(pInfo.pModel);


			if (modelName.find("arms") != std::string::npos)
			{
				if (chamsWireFrameHands || chamsNoHands)
				{
					IMaterial *mat = materialChams;

					if (chamsWireFrameHands)
						mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
					else
						mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, false);

					if (chamsNoHands)
						mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
					else
						mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);


					mat->ColorModulate(chamsHands[0], chamsHands[1], chamsHands[2]);

					ModelRender->ForcedMaterialOverride(mat);
				}
			}
			else if (modelName.find("models/player") != std::string::npos)
			{
				IMaterial *visible_material;
				IMaterial *hidden_material;
				visible_material = materialChamsFlat;
				hidden_material = materialChamsFlatIgnorez;
				C_CSPlayer* player = static_cast<C_CSPlayer*>(EntitiyList->GetClientEntity(pInfo.entity_index));
				auto pLocal = C_CSPlayer::GetLocalPlayer();

				if (pLocal && player)
				{
					if (player != pLocal)
					{

						if (player->GetTeamNum() == pLocal->GetTeamNum())
						{
							visible_material->ColorModulate(chamsAllyColor_Visible[0], chamsAllyColor_Visible[1], chamsAllyColor_Visible[2] );
							hidden_material->ColorModulate(chamsAllyColor[0], chamsAllyColor[1], chamsAllyColor[2]);
						}
						else
						{
							visible_material->ColorModulate(chamsEnemyColor_Visible[0], chamsEnemyColor_Visible[1], chamsEnemyColor_Visible[2]);
							hidden_material->ColorModulate(chamsEnemyColor[0], chamsEnemyColor[1], chamsEnemyColor[2]);
						}

						ModelRender->ForcedMaterialOverride(hidden_material);
						g_ModelRendererHook->GetOriginal<DrawModelExecute_t>(21)(ModelRender, ctx, state, pInfo, pCustomBoneToWorld);

						ModelRender->ForcedMaterialOverride(visible_material);
						g_ModelRendererHook->GetOriginal<DrawModelExecute_t>(21)(ModelRender, ctx, state, pInfo, pCustomBoneToWorld);
					}
				}
			}
		}
	}
}