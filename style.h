#include <imgui.h>
#include <corecrt_math.h>
#include <imgui.h>
class EditorColorScheme
{
	// 0xRRGGBBAA
	inline static int BackGroundColor = 0x2424230F;
	inline static int TextColor = 0xf5cb5c0F;
	inline static int MainColor = 0x3335330F;
	inline static int MainAccentColor = 0x403d390F;
	inline static int HighlightColor = 0xADB5BD0F;

	inline static int Black = 0x00000000;
	inline static int White = 0xFFFFFF00;

	inline static int AlphaTransparent = 0x00;
	inline static int Alpha20 = 0x33;
	inline static int Alpha40 = 0x66;
	inline static int Alpha50 = 0x80;
	inline static int Alpha60 = 0x99;
	inline static int Alpha80 = 0xCC;
	inline static int Alpha90 = 0xE6;
	inline static int AlphaFull = 0xFF;

	static float GetR(int colorCode) { return (float)((colorCode & 0xFF000000) >> 24) / (float)(0xFF); }
	static float GetG(int colorCode) { return (float)((colorCode & 0x00FF0000) >> 16) / (float)(0xFF); }
	static float GetB(int colorCode) { return (float)((colorCode & 0x0000FF00) >> 8) / (float)(0xFF); }
	static float GetA(int alphaCode) { return ((float)alphaCode / (float)0xFF); }

	static ImVec4 GetColor(int c, int a = Alpha90) { return ImVec4(GetR(c), GetG(c), GetB(c), GetA(a)); }
	static ImVec4 Darken(ImVec4 c, float p) { return ImVec4(fmax(0.f, c.x - 1.0f * p), fmax(0.f, c.y - 1.0f * p), fmax(0.f, c.z - 1.0f * p), c.w); }
	static ImVec4 Lighten(ImVec4 c, float p) { return ImVec4(fmax(0.f, c.x + 1.0f * p), fmax(0.f, c.y + 1.0f * p), fmax(0.f, c.z + 1.0f * p), c.w); }

	static ImVec4 Disabled(ImVec4 c) { return Darken(c, 0.6f); }
	static ImVec4 Hovered(ImVec4 c) { return Lighten(c, 0.2f); }
	static ImVec4 Active(ImVec4 c) { return Lighten(ImVec4(c.x, c.y, c.z, 1.0f), 0.1f); }
	static ImVec4 Collapsed(ImVec4 c) { return Darken(c, 0.2f); }

public:

	static void SetColors(int backGroundColor, int textColor, int mainColor, int mainAccentColor, int highlightColor)
	{
		BackGroundColor = backGroundColor;
		TextColor = textColor;
		MainColor = mainColor;
		MainAccentColor = mainAccentColor;
		HighlightColor = highlightColor;
	}
	
	static bool MySwitch(const char* label, bool* v)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const ImVec2 switch_size = ImVec2(label_size.y * 2.0f, label_size.y);

		const ImRect total_bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + switch_size.x, window->DC.CursorPos.y + switch_size.y));
		ImGui::ItemSize(total_bb, style.FramePadding.y);

		if (!ImGui::ItemAdd(total_bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);

		// Render switch background
		ImGui::RenderFrame(total_bb.Min, total_bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, switch_size.y * 0.5f);

		// Calculate switch inner circle position
		const float circle_radius = (switch_size.y - style.FramePadding.y) * 0.5f;
		const float circle_pos = total_bb.Min.x + style.FramePadding.x + circle_radius + (*v ? (switch_size.x - style.FramePadding.x * 2.0f - circle_radius * 2.0f) : 0.0f);

		// Render switch inner circle
		ImGui::RenderFrame(ImVec2(circle_pos - circle_radius, total_bb.Min.y + circle_radius), ImVec2(circle_pos + circle_radius, total_bb.Max.y - circle_radius), ImGui::GetColorU32(*v ? ImGuiCol_ButtonActive : ImGuiCol_Button), true, circle_radius);

		// Render label
		const ImVec2 text_pos = ImVec2(total_bb.Max.x + style.ItemInnerSpacing.x, total_bb.Min.y + style.FramePadding.y);
		ImGui::RenderText(text_pos, label);

		if (pressed)
			*v = !(*v);

		return pressed;
	}

	static void ApplyTheme2()
	{
		static float rainbow;
		rainbow += 0.0010f; //0.0001f
		if (rainbow > 1.f)
			rainbow = 0.f;

		auto rgb = ImColor::HSV(rainbow, 1.f, 1.f, 0.8f);     // Rainbow Items
		auto rgb_a = ImColor::HSV(rainbow, 1.f, 1.f, 0.9f);   // Selected / Hovered Items
		auto rgb_b = ImColor::HSV(rainbow, 1.f, 1.f, 0.7f);   // Pressed Items

		//CAUTION

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = customization::text;
		colors[ImGuiCol_TextDisabled] = customization::text_disabled;
		colors[ImGuiCol_WindowBg] = customization::windowbg;
		colors[ImGuiCol_ChildBg] = ImVec4(0.22f, 0.22f, 0.22f, 0.50f);;
		colors[ImGuiCol_PopupBg] = customization::popup;
		colors[ImGuiCol_Border] = customization::border;
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = customization::framebg;
		colors[ImGuiCol_FrameBgHovered] = customization::framebgh;
		colors[ImGuiCol_FrameBgActive] = customization::framebgh;
		colors[ImGuiCol_TitleBg] =ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = customization::checkmark;
		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.67f, 0.06f, 0.06f, 0.67f);
		colors[ImGuiCol_Button] = customization::button;
		colors[ImGuiCol_ButtonHovered] = customization::buttonh;
		colors[ImGuiCol_ButtonActive] = customization::buttona;
		colors[ImGuiCol_Header] = customization::header;
		colors[ImGuiCol_HeaderHovered] = customization::headerh;
		colors[ImGuiCol_HeaderActive] = customization::headera;
		colors[ImGuiCol_Separator] = ImVec4(1.00f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(1.00f, 0.33f, 0.33f, 0.40f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.67f, 0.06f, 0.06f, 0.67f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.02f, 0.02f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.33f, 0.33f, 0.40f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.67f, 0.06f, 0.06f, 0.67f);
		colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.58f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(1.00f, 0.33f, 0.33f, 0.40f);
		colors[ImGuiCol_TabActive] = ImVec4(0.67f, 0.06f, 0.06f, 0.67f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.0f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);



		ImGuiStyle* style = &ImGui::GetStyle();
	/**/	style->WindowPadding = ImVec2(12.f, 11.f);
	/**/	style->FramePadding = ImVec2(7.f, 3.f);
		style->ItemSpacing = ImVec2(6.f, 3.f);
		style->ItemInnerSpacing = ImVec2(7.f, 4.f);
		style->GrabMinSize = 17.f;
		style->WindowBorderSize = customization::wbs;
		style->ChildBorderSize = 1.f;
	/**/	style->FrameBorderSize = 1.f;
		style->TabBorderSize = 1.f;
		style->WindowRounding = customization::wr;
		style->ChildRounding = 6.f;
		style->FrameRounding = 6.f;
		style->GrabRounding = 6.f;

	}
};