#pragma once
#include <Hookers.h>
namespace SourceEngine
{
	
	float misc_NoFlash = 255;
	float misc_ViewModelFov = 90;
	float misc_Fov = 90;

	void misc_frameStageNotify(ClientFrameStage_t stage)
	{

		C_CSPlayer* localPlayer = static_cast<C_CSPlayer*>(C_CSPlayer::GetLocalPlayer());
		if (!localPlayer) return;

		*localPlayer->GetMaxAlpha() = 255.0f - ( 255 - misc_NoFlash);
		
	}


}