#pragma once


#include "ImGUI\imgui.h"
namespace SourceEngine
{

	//Drawing shit
	HFont font;

	void DrawString(int x, int y, const Color& color, const wchar_t* text)
	{
		if (!text)
			return;

		Surface->DrawSetTextPos(x, y);
		Surface->DrawSetTextFont(font);
		Surface->DrawSetTextColor(color);
		Surface->DrawPrintText(text, wcslen(text));

	}

	void SetupFont()
	{
		font = Surface->CreateFont();
		Surface->SetFontGlyphSet(font, "Tahoma", 12, 255, 0, 0, (int)FontFlags::FONTFLAG_OUTLINE, 0, 0);
	}

	void DrawRect(int x, int y, int width, int height, Color& color)
	{
		Surface->DrawSetColor(color);
		Surface->DrawOutlinedRect(x, y, x + width, y + height);
	}

	void DrawCircle(int x, int y, int radius, Color& color)
	{
		Surface->DrawSetColor(color);
		Surface->DrawOutlinedCircle(x, y, radius, 128);
	}

	void DrawLine(int x1, int y1, int x2, int y2, Color& color)
	{
		Surface->DrawSetColor(color);
		Surface->DrawLine(x1, y1, x2, y2);
	}

	void DrawRectF(int x, int y, int width, int height, Color& color)
	{
		Surface->DrawSetColor(color);
		Surface->DrawFilledRect(x, y, x + width, y + height);
	}

	Vector2D GetTextSize(const wchar_t* text, HFont font)
	{
		int wide = 0;
		int tall = 0;

		Surface->GetTextSize(font, text, wide, tall);

		return Vector2D(wide, tall);
	}

	// [src] https://github.com/ocornut/imgui/issues/346
	// v2



	static bool ColorPicker(float *col, bool alphabar)
	{
		const int    EDGE_SIZE = 200; // = int( ImGui::GetWindowWidth() * 0.75f );
		const ImVec2 SV_PICKER_SIZE = ImVec2(EDGE_SIZE, EDGE_SIZE);
		const float  SPACING = ImGui::GetStyle().ItemInnerSpacing.x;
		const float  HUE_PICKER_WIDTH = 20.f;
		const float  CROSSHAIR_SIZE = 7.0f;

		ImColor color(col[0], col[1], col[2]);
		bool value_changed = false;

		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		// setup

		ImVec2 picker_pos = ImGui::GetCursorScreenPos();

		float hue, saturation, value;
		ImGui::ColorConvertRGBtoHSV(
			color.Value.x, color.Value.y, color.Value.z, hue, saturation, value);

		// draw hue bar

		ImColor colors[] = { ImColor(255, 0, 0),
			ImColor(255, 255, 0),
			ImColor(0, 255, 0),
			ImColor(0, 255, 255),
			ImColor(0, 0, 255),
			ImColor(255, 0, 255),
			ImColor(255, 0, 0) };

		for (int i = 0; i < 6; ++i)
		{
			draw_list->AddRectFilledMultiColor(
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING, picker_pos.y + i * (SV_PICKER_SIZE.y / 6)),
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING + HUE_PICKER_WIDTH,
					picker_pos.y + (i + 1) * (SV_PICKER_SIZE.y / 6)),
				colors[i],
				colors[i],
				colors[i + 1],
				colors[i + 1]);
		}

		draw_list->AddLine(
			ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING - 2, picker_pos.y + hue * SV_PICKER_SIZE.y),
			ImVec2(picker_pos.x + SV_PICKER_SIZE.x + SPACING + 2 + HUE_PICKER_WIDTH, picker_pos.y + hue * SV_PICKER_SIZE.y),
			ImColor(255, 255, 255));

		// draw alpha bar

		if (alphabar) {
			float alpha = col[3];

			draw_list->AddRectFilledMultiColor(
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * SPACING + HUE_PICKER_WIDTH, picker_pos.y),
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * SPACING + 2 * HUE_PICKER_WIDTH, picker_pos.y + SV_PICKER_SIZE.y),
				ImColor(0, 0, 0), ImColor(0, 0, 0), ImColor(255, 255, 255), ImColor(255, 255, 255));

			draw_list->AddLine(
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * (SPACING - 2) + HUE_PICKER_WIDTH, picker_pos.y + alpha * SV_PICKER_SIZE.y),
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x + 2 * (SPACING + 2) + 2 * HUE_PICKER_WIDTH, picker_pos.y + alpha * SV_PICKER_SIZE.y),
				ImColor(255.f - alpha, 255.f, 255.f));
		}

		// draw color matrix

		{
			const ImU32 c_oColorBlack = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 1.f));
			const ImU32 c_oColorBlackTransparent = ImGui::ColorConvertFloat4ToU32(ImVec4(0.f, 0.f, 0.f, 0.f));
			const ImU32 c_oColorWhite = ImGui::ColorConvertFloat4ToU32(ImVec4(1.f, 1.f, 1.f, 1.f));

			ImVec4 cHueValue(1, 1, 1, 1);
			ImGui::ColorConvertHSVtoRGB(hue, 1, 1, cHueValue.x, cHueValue.y, cHueValue.z);
			ImU32 oHueColor = ImGui::ColorConvertFloat4ToU32(cHueValue);

			draw_list->AddRectFilledMultiColor(
				ImVec2(picker_pos.x, picker_pos.y),
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y),
				c_oColorWhite,
				oHueColor,
				oHueColor,
				c_oColorWhite
			);

			draw_list->AddRectFilledMultiColor(
				ImVec2(picker_pos.x, picker_pos.y),
				ImVec2(picker_pos.x + SV_PICKER_SIZE.x, picker_pos.y + SV_PICKER_SIZE.y),
				c_oColorBlackTransparent,
				c_oColorBlackTransparent,
				c_oColorBlack,
				c_oColorBlack
			);
		}

		// draw cross-hair

		float x = saturation * SV_PICKER_SIZE.x;
		float y = (1 - value) * SV_PICKER_SIZE.y;
		ImVec2 p(picker_pos.x + x, picker_pos.y + y);
		draw_list->AddLine(ImVec2(p.x - CROSSHAIR_SIZE, p.y), ImVec2(p.x - 2, p.y), ImColor(255, 255, 255));
		draw_list->AddLine(ImVec2(p.x + CROSSHAIR_SIZE, p.y), ImVec2(p.x + 2, p.y), ImColor(255, 255, 255));
		draw_list->AddLine(ImVec2(p.x, p.y + CROSSHAIR_SIZE), ImVec2(p.x, p.y + 2), ImColor(255, 255, 255));
		draw_list->AddLine(ImVec2(p.x, p.y - CROSSHAIR_SIZE), ImVec2(p.x, p.y - 2), ImColor(255, 255, 255));

		// color matrix logic

		ImGui::InvisibleButton("saturation_value_selector", SV_PICKER_SIZE);

		if (ImGui::IsItemActive() && ImGui::GetIO().MouseDown[0])
		{
			ImVec2 mouse_pos_in_canvas = ImVec2(
				ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

			/**/ if (mouse_pos_in_canvas.x <                     0) mouse_pos_in_canvas.x = 0;
			else if (mouse_pos_in_canvas.x >= SV_PICKER_SIZE.x - 1) mouse_pos_in_canvas.x = SV_PICKER_SIZE.x - 1;

			/**/ if (mouse_pos_in_canvas.y <                     0) mouse_pos_in_canvas.y = 0;
			else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

			value = 1 - (mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1));
			saturation = mouse_pos_in_canvas.x / (SV_PICKER_SIZE.x - 1);
			value_changed = true;
		}

		// hue bar logic

		ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + SPACING + SV_PICKER_SIZE.x, picker_pos.y));
		ImGui::InvisibleButton("hue_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));

		if (ImGui::GetIO().MouseDown[0] && (ImGui::IsItemHovered() || ImGui::IsItemActive()))
		{
			ImVec2 mouse_pos_in_canvas = ImVec2(
				ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

			/**/ if (mouse_pos_in_canvas.y <                     0) mouse_pos_in_canvas.y = 0;
			else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

			hue = mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1);
			value_changed = true;
		}

		// alpha bar logic

		if (alphabar) {

			ImGui::SetCursorScreenPos(ImVec2(picker_pos.x + SPACING * 2 + HUE_PICKER_WIDTH + SV_PICKER_SIZE.x, picker_pos.y));
			ImGui::InvisibleButton("alpha_selector", ImVec2(HUE_PICKER_WIDTH, SV_PICKER_SIZE.y));

			if (ImGui::GetIO().MouseDown[0] && (ImGui::IsItemHovered() || ImGui::IsItemActive()))
			{
				ImVec2 mouse_pos_in_canvas = ImVec2(
					ImGui::GetIO().MousePos.x - picker_pos.x, ImGui::GetIO().MousePos.y - picker_pos.y);

				/**/ if (mouse_pos_in_canvas.y <                     0) mouse_pos_in_canvas.y = 0;
				else if (mouse_pos_in_canvas.y >= SV_PICKER_SIZE.y - 1) mouse_pos_in_canvas.y = SV_PICKER_SIZE.y - 1;

				float alpha = mouse_pos_in_canvas.y / (SV_PICKER_SIZE.y - 1);
				col[3] = alpha;
				value_changed = true;
			}

		}

		// R,G,B or H,S,V color editor

		color = ImColor::HSV(hue >= 1 ? hue - 10 * 1e-6 : hue, saturation > 0 ? saturation : 10 * 1e-6, value > 0 ? value : 1e-6);
		col[0] = color.Value.x;
		col[1] = color.Value.y;
		col[2] = color.Value.z;

		bool widget_used;
		ImGui::PushItemWidth((alphabar ? SPACING + HUE_PICKER_WIDTH : 0) +
			SV_PICKER_SIZE.x + SPACING + HUE_PICKER_WIDTH - 2 * ImGui::GetStyle().FramePadding.x);
		widget_used = alphabar ? ImGui::ColorEdit4("", col) : ImGui::ColorEdit3("", col);
		ImGui::PopItemWidth();

		// try to cancel hue wrap (after ColorEdit), if any
		{
			float new_hue, new_sat, new_val;
			ImGui::ColorConvertRGBtoHSV(col[0], col[1], col[2], new_hue, new_sat, new_val);
			if (new_hue <= 0 && hue > 0) {
				if (new_val <= 0 && value != new_val) {
					color = ImColor::HSV(hue, saturation, new_val <= 0 ? value * 0.5f : new_val);
					col[0] = color.Value.x;
					col[1] = color.Value.y;
					col[2] = color.Value.z;
				}
				else
					if (new_sat <= 0) {
						color = ImColor::HSV(hue, new_sat <= 0 ? saturation * 0.5f : new_sat, new_val);
						col[0] = color.Value.x;
						col[1] = color.Value.y;
						col[2] = color.Value.z;
					}
			}
		}

		return value_changed | widget_used;
	}

	bool ColorPicker3(float col[3]) {
		return ColorPicker(col, false);
	}

	bool ColorPicker4(float col[4]) {
		return ColorPicker(col, true);
	}
}