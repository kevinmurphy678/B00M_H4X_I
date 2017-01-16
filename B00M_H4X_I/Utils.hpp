#pragma once

#include <Windows.h>
#include <Psapi.h>
#include <tlhelp32.h>  
#include "SourceEngine/SDK.hpp"
#include "CSGOStructs.hpp"
#include "Hookers.h"

class Utils
{


public:
	static SourceEngine::Vector GetEntityBone(SourceEngine::IClientEntity* pEntity, SourceEngine::ECSPlayerBones Bone)
	{
		SourceEngine::matrix3x4_t boneMatrix[128];

		if (!pEntity->SetupBones(boneMatrix, 128, 0x00000100, SourceEngine::Interfaces::Engine()->GetLastTimeStamp()))
			return SourceEngine::Vector();

		SourceEngine::matrix3x4_t hitbox = boneMatrix[Bone];

		return SourceEngine::Vector(hitbox[0][3], hitbox[1][3], hitbox[2][3]);
	}

	static bool ScreenTransform(const SourceEngine::Vector& point, SourceEngine::Vector& screen)
	{
		const SourceEngine::VMatrix& w2sMatrix = SourceEngine::Interfaces::Engine()->WorldToScreenMatrix();
		screen.x = w2sMatrix.m[0][0] * point.x + w2sMatrix.m[0][1] * point.y + w2sMatrix.m[0][2] * point.z + w2sMatrix.m[0][3];
		screen.y = w2sMatrix.m[1][0] * point.x + w2sMatrix.m[1][1] * point.y + w2sMatrix.m[1][2] * point.z + w2sMatrix.m[1][3];
		screen.z = 0.0f;

		float w = w2sMatrix.m[3][0] * point.x + w2sMatrix.m[3][1] * point.y + w2sMatrix.m[3][2] * point.z + w2sMatrix.m[3][3];

		if (w < 0.001f) {
			screen.x *= 100000;
			screen.y *= 100000;
			return true;
		}

		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;

		return false;
	}

	
	static bool WorldToScreen(const SourceEngine::Vector &origin, SourceEngine::Vector &screen)
	{
		if (!ScreenTransform(origin, screen)) {
			int iScreenWidth, iScreenHeight;
			SourceEngine::Interfaces::Engine()->GetScreenSize(iScreenWidth, iScreenHeight);

			screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;
			screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;

			return true;
		}
		return false;
	}

	static uint64_t FindSignature(const char* szModule, const char* szSignature)
	{
#define INRANGE(x,a,b)  (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

		MODULEINFO modInfo;
		GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(szModule), &modInfo, sizeof(MODULEINFO));
		DWORD startAddress = (DWORD)modInfo.lpBaseOfDll;
		DWORD endAddress = startAddress + modInfo.SizeOfImage;
		const char* pat = szSignature;
		DWORD firstMatch = 0;
		for (DWORD pCur = startAddress; pCur < endAddress; pCur++) {
			if (!*pat) return firstMatch;
			if (*(PBYTE)pat == '\?' || *(BYTE*)pCur == getByte(pat)) {
				if (!firstMatch) firstMatch = pCur;
				if (!pat[2]) return firstMatch;
				if (*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?') pat += 3;
				else pat += 2;    //one ?
			}
			else {
				pat = szSignature;
				firstMatch = 0;
			}
		}
		return NULL;
	}

	static DWORD Utils::FindPattern(char* pattern, char* mask, DWORD start, DWORD end, DWORD offset)
	{
		int patternLength = strlen(mask);
		bool found = false;

		//For each byte from start to end
		for (DWORD i = start; i < end - patternLength; i++)
		{
			found = true;
			//For each byte in the pattern
			for (int idx = 0; idx < patternLength; idx++)
			{
				if (mask[idx] == 'x' && pattern[idx] != *(char*)(i + idx))
				{
					found = false;
					break;
				}
			}
			if (found)
			{
				return i + offset;
			}
		}
		return NULL;
	}

	static DWORD Utils::GetModuleSize(char* moduleName)
	{
		HANDLE hSnap;
		MODULEENTRY32 xModule;
		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(GetCurrentProcess()));
		xModule.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnap, &xModule)) {
			while (Module32Next(hSnap, &xModule)) {
				if (!strncmp((char*)xModule.szModule, moduleName, 8)) {
					CloseHandle(hSnap);
					return (DWORD)xModule.modBaseSize;
				}
			}
		}
		CloseHandle(hSnap);
		return 0;
	}

	static DWORD Utils::GetModuleBase(char* moduleName)
	{
		HANDLE hSnap;
		MODULEENTRY32 xModule;
		hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(GetCurrentProcess()));
		xModule.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnap, &xModule)) {
			while (Module32Next(hSnap, &xModule)) {
				if (!strncmp((char*)xModule.szModule, moduleName, 8)) {
					CloseHandle(hSnap);
					return *(DWORD*)xModule.modBaseAddr;
				}
			}
		}
		CloseHandle(hSnap);
		return 0;
	}

	static bool Clamp(SourceEngine::QAngle &angles)
	{
		SourceEngine::QAngle a = angles;
		Normalize(a);
		ClampAngles(a);

		if (isnan(a.x) || isinf(a.x) ||
			isnan(a.y) || isinf(a.y) ||
			isnan(a.z) || isinf(a.z)) {
			return false;
		}
		else {
			angles = a;
			return true;
		}
	}
public:
	static void Normalize(SourceEngine::QAngle& angle)
	{
		while (angle.x > 89.0f) {
			angle.x -= 180.f;
		}
		while (angle.x < -89.0f) {
			angle.x += 180.f;
		}
		while (angle.y > 180.f) {
			angle.y -= 360.f;
		}
		while (angle.y < -180.f) {
			angle.y += 360.f;
		}
	}

	static float Rad2Deg(float x)
	{
		return (x * (180.0f / 3.141592654f));
	}
	static float Deg2Rad(float x)
	{
		return (x * (3.141592654f / 180.0f));
	}

	static void ClampAngles(SourceEngine::QAngle &angles)
	{
		if (angles.y > 180.0f)
			angles.y = 180.0f;
		else if (angles.y < -180.0f)
			angles.y = -180.0f;

		if (angles.x > 89.0f)
			angles.x = 89.0f;
		else if (angles.x < -89.0f)
			angles.x = -89.0f;

		angles.z = 0;
	}

	static void inline SinCos(float radians, float *sine, float *cosine)
	{
		*sine = sin(radians);
		*cosine = cos(radians);
	}

	static void AngleVectors(const QAngle &angles, Vector& forward)
	{
		float sp, sy, cp, cy;

		SinCos(Deg2Rad(angles[1]), &sy, &cy);
		SinCos(Deg2Rad(angles[0]), &sp, &cp);

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	static void VectorAngles(const SourceEngine::Vector& forward, SourceEngine::Vector& angles)
	{
		float tmp, yaw, pitch;

		if (forward.y == 0 && forward.x == 0)
		{
			yaw = 0;

			if (forward.z > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else
		{
			yaw = Rad2Deg(atan2f(forward.y, forward.x));

			if (yaw < 0)
				yaw += 360;

			tmp = forward.Length2D();
			pitch = Rad2Deg(atan2f(-forward.z, tmp));

			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}

	static void MakeVector(const SourceEngine::Vector& vIn, SourceEngine::Vector& vOut)
	{
		float pitch = Deg2Rad(vIn.x);
		float yaw = Deg2Rad(vIn.y);
		float temp = cos(pitch);

		vOut.x = -temp * -cos(yaw);
		vOut.y = sin(yaw) * temp;
		vOut.z = -sin(pitch);
	}

	static float GetFOV(const SourceEngine::Vector& viewangles, const SourceEngine::Vector& vStart, const SourceEngine::Vector& vEnd)
	{
		SourceEngine::Vector vAng, vAim;

		SourceEngine::Vector vDir = vEnd - vStart;

		vDir = vDir.Normalized();

		VectorAngles(vDir, vAng);

		MakeVector(viewangles, vAim);
		MakeVector(vAng, vAng);

		return Rad2Deg(acos(vAim.Dot(vAng)) / vAim.LengthSqr());
	}
	static bool Utils::IsButtonPressed(ButtonCode_t code)
	{
		static int buttonPressedTick = 0;

		if (SourceEngine::Input->IsButtonDown(code) && (GetTickCount64() - buttonPressedTick) > 300)
		{
			buttonPressedTick = GetTickCount64();
			return true;
		}
		return false;
	}


};

