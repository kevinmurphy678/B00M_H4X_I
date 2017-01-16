#pragma once
#include <SourceEngine/SDK.hpp>
#include <CSGOStructs.hpp>
#include <VFTableHook.hpp>
#include <memory.h>
#include <InterfaceManager.h>
#include <Utils.hpp>
#include <Windows.h>
#include <math.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "ImGUI/imgui.h"
#include "ImGUI/DX9/imgui_impl_dx9.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace SourceEngine
{

	using InitKeyValuesFn = void(__thiscall*)(void* thisptr, const char* name);
	using LoadFromBufferFn = void(__thiscall*)(void* thisptr, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc);
	static InitKeyValuesFn InitKeyValuesEx;
	static LoadFromBufferFn LoadFromBufferEx;

    void InitKeyValues(KeyValues* pKeyValues, const char* name);
	void LoadFromBuffer(KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc);

	void Init();
	void Unload();


	typedef bool(__thiscall* CreateMove_t)(SourceEngine::IClientMode*, float, SourceEngine::CUserCmd*);
	typedef void(__thiscall* hkPaintTraverse_t)(SourceEngine::IPanel* Panel__, unsigned int panel, bool forceRepaint, bool allowForce);
	typedef long(__stdcall* EndScene_t)(IDirect3DDevice9* device);
	typedef long(__stdcall* Reset_t)(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pp);
	typedef void(__thiscall* FrameStageNotify_t)(void*, SourceEngine::ClientFrameStage_t);
	//typedef void(__stdcall* OverrideView_t)(void*, CViewSetup* view);
	typedef void(__thiscall* RenderView_t)(void*, CViewSetup&, CViewSetup&, int, int);

	typedef void(__thiscall*  DrawModelExecute_t)(
		IVModelRender*,
		IMatRenderContext*,
		const DrawModelState_t&,
		const ModelRenderInfo_t&,
		matrix3x4_t*);

	typedef float(__stdcall *GetViewModelFov_t)();

	float __stdcall GetViewModelFov();
	//void __fastcall OverrideView_Hooker(void* ecx, int edx, CViewSetup* view);

	void __stdcall DrawModelExecute(
		IMatRenderContext* ctx,
		const DrawModelState_t &state,
		const ModelRenderInfo_t &pInfo,
		matrix3x4_t *pCustomBoneToWorld);

	void __fastcall RenderView_Hooker(void* thisptr, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);

	bool      __stdcall CreateMove_Hooker(float sample_input_frametime, SourceEngine::CUserCmd* pCmd);
	void      __stdcall khPaintTraverse_Hooker(unsigned int panel, bool forceRepaint, bool allowForce);
	void __fastcall frameStageNotify(void* ecx, void* edx, SourceEngine::ClientFrameStage_t stage);
	HRESULT   __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice);
	HRESULT   __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	LRESULT   __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static HMODULE moduleFFFF;




	IMaterial* CreateMaterial(std::string type, std::string texture, bool ignorez, bool nofog, bool model, bool nocull, bool halflambert);

}

