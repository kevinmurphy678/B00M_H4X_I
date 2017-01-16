#include <Hookers.h>
#include<Draw.hpp>
#include<Esp.hpp>
#include<Aimbot.hpp>
#include<Misc.hpp>
#include<Chams.hpp>
#include <DrawManager.hpp>
#include "ImGUI/imgui.h"
#include "ImGUI/DX9/imgui_impl_dx9.h"

using namespace SourceEngine;
namespace SourceEngine
{
	bool running = true;
	DWORD clientBase;


	

	CreateMove_t                       CreateMove_Original = nullptr;
	hkPaintTraverse_t				   PaintTraverse_Original = nullptr;
	EndScene_t                         g_fnOriginalEndScene = nullptr;
	Reset_t                            g_fnOriginalReset = nullptr;
	FrameStageNotify_t				   frameStage_Original = nullptr;
	//OverrideView_t					   OverrideView_Original = nullptr;
	GetViewModelFov_t				   GetViewModel_Original = nullptr;
	RenderView_t					   RenderView_Original = nullptr;
	DrawModelExecute_t				   DrawModelExecute_Original = nullptr;

	bool bunnyE = false;

	//Configs
	bool menu_Open = false;


	HWND                               g_hWindow = nullptr; //Handle to the CSGO window
	bool                               g_bWasInitialized = false;
	WNDPROC                            g_pOldWindowProc = nullptr; //Old WNDPROC pointer
	bool                               vecPressedKeys[256] = {};


	inline uintptr_t GetAbsoluteAddress(uintptr_t instruction_ptr, int offset, int size)
	{
		return instruction_ptr + *reinterpret_cast<uint32_t*>(instruction_ptr + offset) + size;
	};
	void Unload()
	{
		std::cout << "\n UNLOADING HOOK(ER)S" << std::endl;
		SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, (LONG_PTR)g_pOldWindowProc);

		g_ModelRendererHook->Unhook(21);
		g_ModelRendererHook->RestoreTable();

		running = false;
	}

	DWORD LoadFromBufferEx_o;
	DWORD InitKeyValuesEx_o;

	void Init()
	{
		while (!(g_hWindow = FindWindowA(XorStr("Valve001"), NULL))) Sleep(100);
		if (g_hWindow)
			g_pOldWindowProc = (WNDPROC)SetWindowLongPtr(g_hWindow, GWLP_WNDPROC, (LONG_PTR)Hooked_WndProc);

		NetvarManager::Instance()->CreateDatabase();

		clientBase = Utils::GetModuleBase("client.dll");

		Engine = InterfaceManager::Grab<IVEngineClient>("engine.dll", "VEngineClient014");
		Panel = InterfaceManager::Grab<IPanel>("vgui2.dll", "VGUI_Panel009");
		Surface = InterfaceManager::Grab<ISurface>("vguimatsurface.dll", "VGUI_Surface031");
		Client = InterfaceManager::Grab<IBaseClientDLL>("client.dll", "VClient018");
		EntitiyList = InterfaceManager::Grab<IClientEntityList>("client.dll", "VClientEntityList003");
		EngineTrace = InterfaceManager::Grab<IEngineTrace>("engine.dll", "EngineTraceClient004");
		Input = InterfaceManager::Grab<IInputSystem>("inputsystem.dll", "InputSystemVersion001");

		ModelRender = InterfaceManager::Grab<IVModelRender>("engine.dll", "VEngineModel016");

		ModelInfo = InterfaceManager::Grab<IVModelInfo>("engine.dll", "VModelInfoClient004");
		Material = InterfaceManager::Grab<IMaterialSystem>("materialsystem.dll", "VMaterialSystem080");
		

		void** pClientTable = *reinterpret_cast<void***>(Client);
		ClientMode = **reinterpret_cast<IClientMode***>(reinterpret_cast<DWORD>(pClientTable[10]) + 0x5);
		IPlayerInfoManager* playermanager = InterfaceManager::Grab<IPlayerInfoManager>("server.dll", "PlayerInfoManager002");
		GlobalVars = playermanager->GetGlobalVars();

		//client mode shit
		g_pClientModeHook = std::make_unique<VFTableHook>((PPDWORD)ClientMode, true);
		CreateMove_Original = g_pClientModeHook->Hook(24, (CreateMove_t)CreateMove_Hooker);
		//OverrideView_Original = g_pClientModeHook->Hook(18, (OverrideView_t)OverrideView_Hooker); BROKEN
		GetViewModel_Original = g_pClientModeHook->Hook(35, (GetViewModelFov_t)GetViewModelFov);

		//PaintTraverse
		g_pPaintTraverseHook = std::make_unique<VFTableHook>((PPDWORD)Panel, true);
		PaintTraverse_Original = g_pPaintTraverseHook->Hook(41, (hkPaintTraverse_t)khPaintTraverse_Hooker);

		//FrameStageNotify
		g_frameStageNotify = std::make_unique<VFTableHook>((PPDWORD)Client, true);
		frameStage_Original = g_frameStageNotify->Hook(36, (FrameStageNotify_t)frameStageNotify);

		//DrawModelExecute
		g_ModelRendererHook = std::make_unique<VFTableHook>((PPDWORD)ModelRender, true);
		DrawModelExecute_Original = g_ModelRendererHook->Hook(21, (DrawModelExecute_t)DrawModelExecute);

		//D3D
		auto dwDevice = **(uint32_t**)(Utils::FindSignature(XorStr("shaderapidx9.dll"), XorStr("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);
		g_pD3DDevice9Hook = std::make_unique<VFTableHook>((PPDWORD)dwDevice, true);
		g_fnOriginalReset = g_pD3DDevice9Hook->Hook(16, Hooked_Reset);                 
		g_fnOriginalEndScene = g_pD3DDevice9Hook->Hook(42, Hooked_EndScene);    

		//View render
		auto viewRender = **reinterpret_cast<void***>(Utils::FindSignature("client.dll", "FF 50 14 E8 ?? ?? ?? ?? 5D") - 7);
		g_RenderViewHook = std::make_unique<VFTableHook>((PPDWORD)viewRender, true);
		RenderView_Original = g_RenderViewHook->Hook(6, (RenderView_t)RenderView_Hooker);

		SetupFont();

		LoadFromBufferEx_o = Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89 4C 24 04"));
		InitKeyValuesEx_o = Utils::FindSignature(XorStr("client.dll"), XorStr("55 8B EC 51 33 C0 C7 45"));

		//typedef void(__fastcall* SendClanTag_t)(const char*,const char*);
		//static auto SendClanTag = reinterpret_cast<SendClanTag_t>(Utils::FindSignature(XorStr("engine.dll"), XorStr("53 56 57 8B DA 8B F9 FF 15")));
		//if (pcmd&&pcmd->command_number) { SendClanTag("[VALV\xE1\xB4\xB1]", ""); }




	
	}


	void InitKeyValues(KeyValues* pKeyValues, const char* name)
	{
		InitKeyValuesEx = (InitKeyValuesFn)(InitKeyValuesEx_o);
		InitKeyValuesEx(pKeyValues, name);
	}

	void LoadFromBuffer(KeyValues* pKeyValues, const char* resourceName, const char* pBuffer, void* pFileSystem, const char* pPathID, void* pfnEvaluateSymbolProc)
	{
		LoadFromBufferEx = (LoadFromBufferFn)(LoadFromBufferEx_o);
		LoadFromBufferEx(pKeyValues, resourceName, pBuffer, pFileSystem, pPathID, pfnEvaluateSymbolProc);
	}


	void __fastcall frameStageNotify(void* ecx, void* edx, SourceEngine::ClientFrameStage_t stage)
	{
		if (!Engine->IsInGame()) return;

		misc_frameStageNotify(stage);
		frameStage_Original(ecx, stage);
	}

	float oldx = 0;
	float oldy = 0;

	//Create move hooker
	bool __stdcall CreateMove_Hooker(float sample_input_frametime, CUserCmd* pCmd)
	{
		bool orig = CreateMove_Original(ClientMode, sample_input_frametime, pCmd);
		if (pCmd->command_number == 0)
			return false;

		auto pLocal = C_CSPlayer::GetLocalPlayer();

		if (!pLocal)return false;

		if (bunnyE)
		{
			if ((pCmd->buttons & IN_JUMP) && !(pLocal->GetFlags() & (int)SourceEngine::EntityFlags::FL_ONGROUND))
				pCmd->buttons &= ~IN_JUMP;
		}

		CreateMove_Aimbot(sample_input_frametime, pCmd);

		return true;
	}

	//Paint Hooker
	void __stdcall khPaintTraverse_Hooker(unsigned int panel, bool forceRepaint, bool allowForce)
	{
		PaintTraverse_Original(Panel, panel, forceRepaint, allowForce);

		static unsigned int topPanelId = 0;
		if (!topPanelId)
		{
			if (!strcmp(Panel->GetName(panel), "MatSystemTopPanel"))
			{
				topPanelId = panel;

			}
		}
		else if (panel == topPanelId)
		{
			if (!Engine->IsInGame())
			{
				DrawString(5, 5, Color(255, 255, 255), L"B00M_H4X INJECTED");

			}
			else
			{
				DrawESP();
				PaintTraverse_Aimbot(panel, forceRepaint, allowForce);
			}
		}
	}


	void __stdcall DrawModelExecute(
		IMatRenderContext* ctx,
		const DrawModelState_t &state,
		const ModelRenderInfo_t &pInfo,
		matrix3x4_t *pCustomBoneToWorld)
	{

		Chams_DrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

		DrawModelExecute_Original(ModelRender, ctx, state, pInfo, pCustomBoneToWorld);
		ModelRender->ForcedMaterialOverride(NULL);

	}
	

	void __fastcall RenderView_Hooker(void* thisptr, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
	{
		setup.fov = misc_Fov;
		RenderView_Original(thisptr, setup, hudViewSetup, nClearFlags, whatToDraw);
	}

	float __stdcall GetViewModelFov()
	{
		return misc_ViewModelFov;
	}

	void GUI_Init(IDirect3DDevice9* pDevice)
	{
		//Initializes the GUI and the renderer
		printf("Initiated D3D Device: "); 
		
		bool suc = ImGui_ImplDX9_Init(g_hWindow, pDevice);

		if (suc)printf("True\n"); else printf("False\n");

		g_pRenderer = std::make_unique<DrawManager>(pDevice);
		g_pRenderer->CreateObjects();
		g_bWasInitialized = true;

		ImGuiStyle * style = &ImGui::GetStyle();

		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 5.0f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
		style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
		style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	}

	LRESULT __stdcall Hooked_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg) {
		case WM_LBUTTONDOWN:
			vecPressedKeys[VK_LBUTTON] = true;
			break;
		case WM_LBUTTONUP:
			vecPressedKeys[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONDOWN:
			vecPressedKeys[VK_RBUTTON] = true;
			break;
		case WM_RBUTTONUP:
			vecPressedKeys[VK_RBUTTON] = false;
			break;
		case WM_KEYDOWN:
			vecPressedKeys[wParam] = true;
			break;
		case WM_KEYUP:
			vecPressedKeys[wParam] = false;
			break;
		default: break;
		}

		static bool isDown = false;
		static bool isClicked = false;
		if (vecPressedKeys[VK_F4]) {
			isClicked = false;
			isDown = true;
		}
		else if (!vecPressedKeys[VK_F4] && isDown) {
			isClicked = true;
			isDown = false;
		}
		else {
			isClicked = false;
			isDown = false;
		}

		if (isClicked) {
			menu_Open = !menu_Open;

			//Use cl_mouseenable convar to disable the mouse when the window is open 
			static auto cl_mouseenable = SourceEngine::Interfaces::CVar()->FindVar(XorStr("cl_mouseenable"));
			cl_mouseenable->SetValue(!menu_Open);
		}

		if (g_bWasInitialized && menu_Open && ImGui_ImplDX9_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true; //Input was consumed, return

		return CallWindowProc(g_pOldWindowProc, hWnd, uMsg, wParam, lParam);
	}

	//D3D Endscene, render imgui here
	HRESULT __stdcall Hooked_EndScene(IDirect3DDevice9* pDevice)
	{
		if (!g_bWasInitialized) {
			GUI_Init(pDevice);
		}
		else
		{
			ImGui::GetIO().MouseDrawCursor = menu_Open;
			if (menu_Open)
			{
				//Begins a new ImGui frame.
				ImGui_ImplDX9_NewFrame();
				ImGui::Begin(XorStr("B00M_H4X_INTERNAL"), &menu_Open, ImVec2(300, 250), 1.f);
				{
					if (ImGui::CollapsingHeader("Aimbot"))
					{
						ImGui::Checkbox(XorStr("Aimbot Enabled"), &aimbotE);
						ImGui::Checkbox(XorStr("Auto Shoot"), &aimbotAutoShoot);
						ImGui::SliderFloat("Aimbot FOV", &aimbotFov, 1, 100, "%.0f");
					}
					if (ImGui::CollapsingHeader("Bunny Hop"))
					{
						ImGui::Checkbox(XorStr("Bunnyhop Enabled"), &bunnyE);
					}
					if (ImGui::CollapsingHeader("ESP"))
					{
						ImGui::Checkbox(XorStr("ESP Enabled"), &espE);
						ImGui::Checkbox(XorStr("Line of sight"), &esp_LOS);
						ImGui::Checkbox(XorStr("Skeleton"), &espSkeleton);
						ImGui::Text("Ally Color");
						ColorPicker3(esp_CT_Color);
						ImGui::Text("Enemy Color");
						ColorPicker3(esp_T_Color);
						
					}
					if (ImGui::CollapsingHeader("Chams"))
					{
						ImGui::Checkbox(XorStr("Chams Enabled"), &chamsE);
						ImGui::Checkbox(XorStr("Wireframe Hands"), &chamsWireFrameHands);
						ImGui::Checkbox(XorStr("No Hands"), &chamsNoHands);
						ImGui::Text("Hands Color (Wireframe only)");
						ColorPicker3(chamsHands);
					/*	if (ImGui::CollapsingHeader("Ally Color"))
						{
							ImGui::Text("Visible Color Ally");
							ColorPicker3(chamsAllyColor_Visible);
							ImGui::Text("Hidden Color Ally");
							ColorPicker3(chamsAllyColor);
							
						}

						if (ImGui::CollapsingHeader("Enemy Color"))
						{
							ImGui::Text("Visible Color Enemy");
							ColorPicker3(chamsEnemyColor_Visible);
							ImGui::Text("Hidden Color Enemy");
							ColorPicker3(chamsEnemyColor);			
						}*/

					}
					if (ImGui::CollapsingHeader("Misc"))
					{
						ImGui::SliderFloat("Flash Intensity", &misc_NoFlash, 0, 255, "%.0f");
						ImGui::SliderFloat("Viewmodel FOV", &misc_ViewModelFov, 60, 160, "%.0f");
						ImGui::SliderFloat("FOV", &misc_Fov, 1, 179, "%.0f");
					}
				}
				ImGui::End(); //End main window
				g_pRenderer->BeginRendering();
				ImGui::Render();
				g_pRenderer->EndRendering();
			}
		}
		return g_fnOriginalEndScene(pDevice);
	}

	//D3D Reset, called when alt tabbed, etc
	HRESULT __stdcall Hooked_Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		if (!g_bWasInitialized) return g_fnOriginalReset(pDevice, pPresentationParameters);
		ImGui_ImplDX9_InvalidateDeviceObjects(); //Invalidate GUI resources
		g_pRenderer->InvalidateObjects();
		auto hr = g_fnOriginalReset(pDevice, pPresentationParameters);
		g_pRenderer->CreateObjects();
		ImGui_ImplDX9_CreateDeviceObjects(); //Recreate GUI resources
		return hr;

	}
}

