#include "stdafx.h"
#include "Menu.h"
#include "imgui/imgui.h"
#include "globals.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx12.h"
#include"memory.h"
#include "style.h"
#include <Kiero/kiero.h>
#include "settings.h"
#include "input.h"
#include "game.h"
#include "esp.h"
#include "xor.hpp"
#include "aimbot.h"

bool choice = false;
int Tabs = 0;
bool endKeyPreviouslyPressed = false;
bool enableSnowfall = false;
bool enableCircles = false;
auto vec_files = Settings::GetList();

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

struct Snowflake
{
	ImVec2 position;
	float size;
	float speed;
};
struct Circle
{
	ImVec2 position;
	float radius;
	float duration;
	float timer;
	ImU32 color;
};

float randomFloat(float min, float max)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(min, max);
	return dis(gen);
}

void drawSnowfall()
{
	static std::vector<Snowflake> snowflakes;
	static int maxSnowflakes = 100;

	if (enableSnowfall)
	{
		// Create new snowflakes
		while (snowflakes.size() < maxSnowflakes)
		{
			Snowflake flake;
			flake.position = ImVec2(randomFloat(0, ImGui::GetIO().DisplaySize.x), 0);
			flake.size = randomFloat(2, 5);
			flake.speed = randomFloat(50, 200);
			snowflakes.push_back(flake);
		}
	}

	// Update and draw snowflakes
	if (!snowflakes.empty())
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		for (int i = 0; i < snowflakes.size(); ++i)
		{
			Snowflake& flake = snowflakes[i];
			flake.position.y += flake.speed * ImGui::GetIO().DeltaTime;

			if (flake.position.y > ImGui::GetIO().DisplaySize.y)
			{
				// Remove snowflakes that reach the bottom
				snowflakes.erase(snowflakes.begin() + i);
				--i;
			}
			else
			{
				// Draw snowflake
				drawList->AddCircleFilled(flake.position, flake.size, IM_COL32(255, 255, 255, 255));
			}
		}
	}
}

void drawCircles()
{
	static std::vector<Circle> circles;
	static int maxCircles = 100;

	if (enableCircles)
	{
		// Create new circles
		while (circles.size() < maxCircles)
		{
			Circle circle;
			circle.position = ImVec2(randomFloat(0, ImGui::GetIO().DisplaySize.x), randomFloat(0, ImGui::GetIO().DisplaySize.y));
			circle.radius = randomFloat(0, 80);
			circle.duration = randomFloat(1, 5);
			circle.timer = 0;
			circle.color = IM_COL32(randomFloat(0, 255), randomFloat(0, 255), randomFloat(0, 255), 255);
			circles.push_back(circle);
		}
	}

	// Update and draw circles
	if (!circles.empty())
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		for (int i = 0; i < circles.size(); ++i)
		{
			Circle& circle = circles[i];
			circle.timer += ImGui::GetIO().DeltaTime;

			if (circle.timer > circle.duration)
			{
				// Remove circles that have reached their duration
				circles.erase(circles.begin() + i);
				--i;
			}
			else
			{
				// Calculate current alpha value based on the timer
				float alpha = 1.0f - (circle.timer / circle.duration);
				ImU32 fadedColor = circle.color & IM_COL32(255, 255, 255, static_cast<int>(alpha * 255));

				// Draw circle
				drawList->AddCircle(circle.position, circle.radius, fadedColor, 16, 2.0f);
			}
		}
	}
}

float textPosition = 0.0f;
const float TextSpeed = 100.0f;

ImU32 PackColor(int r, int g, int b, int a)
{
	return static_cast<ImU32>((a << 24) | (b << 16) | (g << 8) | r);
}

void render_SliderFloat(const char* id, float& value, float lineLength, float minValue, float maxValue, ImU32 leftColor, const char* format)
{

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImVec2 linePos = window->DC.CursorPos;
	ImVec2 lineSize(lineLength, 4.0f);

	float circleRadius = 8.0f;
	float lineStartX = linePos.x;
	float lineEndX = linePos.x + lineSize.x;
	float clampedValue = std::clamp(value, minValue, maxValue);
	float circlePosX = lineStartX + (lineEndX - lineStartX) * ((clampedValue - minValue) / (maxValue - minValue));
	ImVec2 circlePos(circlePosX, linePos.y + lineSize.y / 2.0f);

	ImVec2 lineStartPos = linePos;
	ImVec2 lineEndPos = ImVec2(linePos.x + lineSize.x, linePos.y + lineSize.y);

	ImVec2 filledStartPos = lineStartPos;
	ImVec2 filledEndPos = ImVec2(circlePos.x, lineEndPos.y);
	window->DrawList->AddRectFilled(filledStartPos, filledEndPos, leftColor);

	ImVec2 lineStartPosRight = ImVec2(circlePos.x, lineStartPos.y);
	ImVec2 lineEndPosRight = lineEndPos;
	window->DrawList->AddRectFilled(lineStartPosRight, lineEndPosRight, IM_COL32(128, 128, 128, 255));

	ImGui::SetCursorScreenPos(ImVec2(circlePos.x - circleRadius, circlePos.y - circleRadius));
	ImGui::PushID(id); 
	ImGui::InvisibleButton("##circle", ImVec2(circleRadius * 2.0f, circleRadius * 2.0f));
	ImGui::PopID();

	static bool activeSlider = false; 

	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
	{
		if (!activeSlider)
		{
			activeSlider = true;
			value = clampedValue;
		}

		ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
		float deltaValue = (mouseDelta.x / (lineEndX - lineStartX)) * (maxValue - minValue);
		value += deltaValue;
		value = std::clamp(value, minValue, maxValue);
	}
	else if (!ImGui::IsMouseDown(0))
	{
		activeSlider = false;
	}

	window->DrawList->AddCircleFilled(circlePos, circleRadius, IM_COL32(255, 255, 255, 255));

	ImGui::SetCursorScreenPos(ImVec2(linePos.x + lineSize.x / 2.0f, linePos.y - ImGui::GetTextLineHeight() / 2.0f));
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), format, value);
	ImGui::PopFont();
}

namespace g_menu
{
	void snowfall(ImFont* font)
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::Begin("A", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);

		if (enableSnowfall)
			drawSnowfall();

		//if (choice == true && globals::b_menu_open)
		//{

		//	// Get the window dimensions
		//	ImVec2 windowSize = ImGui::GetWindowSize();

		//	// Calculate the position of the scrolling text
		//	ImVec2 textPos(textPosition, windowSize.y - ImGui::GetTextLineHeightWithSpacing() - 10);

		//	// Wrap the text position to the other side when it goes offscreen
		//	if (textPosition > windowSize.x)
		//	{
		//		textPosition = -ImGui::CalcTextSize(XOR("Enjoy the best Legit Features!")).x;
		//	}

		//	esp::g_draw::draw_sketch_edge_text(font, XOR("Enjoy the best Legit Features!"), textPos, 25.0f, ImColor(255, 255, 255, 255), false);

		//	// Update the text position
		//	textPosition += TextSpeed * ImGui::GetIO().DeltaTime;
		//}

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->PushClipRectFullScreen();
		ImGui::End();
		ImGui::PopStyleColor();
	}

	void menu()
	{
		if (GetAsyncKeyState(VK_INSERT) & 0x1)
		{
			globals::b_menu_open = !globals::b_menu_open;
		}
		if (GetAsyncKeyState(VK_END))
		{
			if (!endKeyPreviouslyPressed)
			{
				globals::menu_available = !globals::menu_available;  // Toggle the value
			}
			endKeyPreviouslyPressed = true;
		}
		else
		{
			endKeyPreviouslyPressed = false;
		}
		if (GetAsyncKeyState(VK_PRIOR))
		{
			kiero::unbind(140);
			kiero::shutdown();
		}

		if (GetAsyncKeyState(VK_F1))
		{
			std::string buf = XOR("Profile_1");

			if (!vec_files.empty())
				Settings::Load_Settings(buf);
		}

		if (GetAsyncKeyState(VK_F2))
		{
			std::string profile2 = XOR("Profile_2");

			if (!vec_files.empty())
				Settings::Load_Settings(profile2);
		}

		if (GetAsyncKeyState(VK_F3))
		{
			std::string buf2 = XOR("Profile_3");

			if (!vec_files.empty())
				Settings::Load_Settings(buf2);
		}

		EditorColorScheme::ApplyTheme2();

		if (choice == false && screenshot::visuals)
		{
			ImGui::SetNextWindowSize(ImVec2(180, 100), ImGuiCond_Always);
			ImGui::Begin(XOR("PlatForm Selection"), &globals::b_menu_open, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);

			static int battlenet = 0;
			static int steam = 0;
			static bool animate = true;

			ImGui::Text("Select Platform Please...");
			ImGui::Spacing();
			if (ImGui::Button(XOR("Battle.net"), ImVec2(75, 25)))
				battlenet++;
			ImGui::SameLine();
			if (ImGui::Button(XOR("steam"), ImVec2(75, 25)))
				steam++;

			if (battlenet & 1)
			{
				static float progress = 0.0f, progress_dir = 1.0f;
				if (animate)
				{
					progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
					if (progress >= +1.1f) { progress = +1.1; }
					if (progress <= -0.1f) { progress = -0.1f; }
				}
				ImGui::Spacing();
				ImGui::ProgressBar(progress, ImVec2(125, 22.5f));

				float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
				char buf[32];
				sprintf_s(buf, XOR("%d/%d"), (int)(progress_saturated * 1753), 1753);

				if (progress >= 1.1f)
				{
					globals::platform = 0;
					choice = true;
				}
			}
			else if (steam & 1)
			{
				static float progress = 0.0f, progress_dir = 1.0f;
				if (animate)
				{
					progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
					if (progress >= +1.1f) { progress = +1.1; }
					if (progress <= -0.1f) { progress = -0.1f; }
				}
				ImGui::Spacing();
				ImGui::ProgressBar(progress, ImVec2(125, 22.5f));

				float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
				char buf[32];
				sprintf_s(buf, XOR("%d/%d"), (int)(progress_saturated * 1753), 1753);

				if (progress >= 1.1f)
				{
					globals::platform = 1;
					choice = true;
				}
			}
		}

		if (choice == true && globals::b_menu_open && screenshot::visuals)
		{
			static int activeTab = 0;
			static int previousTab = -1;
			static int previoussubTab = -1;
			static int visualsubTab = 0;

			ImGui::SetNextWindowSize(ImVec2(615, 465));
			ImGui::Begin(XOR(""), &globals::b_menu_open, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize);

			if (enableCircles && globals::b_menu_open)
				drawCircles();
			
			ImGuiStyle* style = &ImGui::GetStyle();
			ImVec4* colors = ImGui::GetStyle().Colors;
			auto previousWP = style->WindowPadding;
			auto previousFBS = style->FrameBorderSize;
			auto previousFR = style->FrameRounding;
			auto previousCBG = colors[ImGuiCol_ChildBg];
			style->WindowPadding = ImVec2(0.f, 0.f);
			style->FrameBorderSize = 0.0f;
			style->FrameRounding = 0.0f;
			colors[ImGuiCol_ChildBg] = ImVec4(0.22f, 0.22f, 0.22f, 0.00f);

			ImGui::SetCursorPos(ImVec2(-1, 0));
			if (ImGui::BeginChild("##TABS", ImVec2(75, 430)), ImGuiWindowFlags_NoBackground)
			{
				ImGui::SetCursorPosY(55.0f);
				if (ImGui::Button("Aimbot", ImVec2(65, 40)))
				{
					previousTab = activeTab;
					activeTab = 0;
				}

				ImGui::Spacing();

				if (ImGui::Button("Visuals", ImVec2(65, 40)))
				{
					previousTab = activeTab;
					activeTab = 1;
					visualsubTab = 0;
				}

				ImGui::Spacing();

				if (ImGui::Button("Misc", ImVec2(65, 40)))
				{
					previousTab = activeTab;
					activeTab = 2;
				}

				if (ImGui::Button("Configs", ImVec2(65, 40)))
				{
					previousTab = activeTab;
					activeTab = 3;
				}

				if (ImGui::Button("Menu S", ImVec2(65, 40)))
				{
					previousTab = activeTab;
					activeTab = 4;
				}
			}
			style->WindowPadding = previousWP;
			style->FrameBorderSize = previousFBS;
			style->FrameRounding = previousFR;
			colors[ImGuiCol_ChildBg] = previousCBG;
			ImGui::EndChild();


			style->WindowPadding = ImVec2(3.f, 3.f);


			switch (activeTab)
			{
			case 0:
				colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.67f);
				style->ChildBorderSize = 0.f;
				ImGui::SetCursorPos(ImVec2(70, 40));
				if (ImGui::BeginChild("##aimbotchildmain", ImVec2(530, 410), true))
				{
					ImGui::SetCursorPos(ImVec2(10, 60));
					ImGui::BeginChild(XOR("##aimbotchild0"), ImVec2(247, 340), true);
					ImGui::Checkbox(XOR("Enable Aimbot"), &globals::b_lock);
					ImGui::Checkbox(XOR("Disable Visible Check"), &globals::b_visible);
					ImGui::Checkbox(XOR("Prediction"), &globals::prediction);
					ImGui::Checkbox(XOR("Disable Before Down"), &globals::disable_bd);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Will Disable aimbot after player loses half of health"));
					ImGui::Checkbox(XOR("Aim at Downed"), &globals::b_skip_knocked);
					ImGui::Checkbox(XOR("Aim at Self Revive"), &globals::self_revive);
					ImGui::Checkbox(XOR("Aim at Being Revived"), &globals::b_revive);
					ImGui::Checkbox(XOR("Show FOV"), &globals::b_fov);
					ImGui::SameLine(215);
					ImGui::ColorEdit4(XOR("##fov Color4"), (float*)&color::showfov, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("FOV Color"));
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::SetCursorPosX(25);
					render_SliderFloat("##FOV_SIZE", globals::f_fov_size, 200, 10.0f, 1200.0f, customization::slider, "FOV Size: %0.0f");
					ImGui::EndChild();

					ImGui::SetCursorPos(ImVec2(275, 60));
					ImGui::BeginChild(XOR("##aimbotchild1"), ImVec2(247, 340), true);
					ImGui::TextDisabled(XOR("Bone Selection"));
					ImGui::Checkbox(XOR("Bone Scan"), &globals::bone_scan);
					ImGui::Checkbox(XOR("Smart Bone Selection"), &globals::smart_bone);
					ImGui::PushItemWidth(200);
					ImGui::Combo(XOR("##MAINBONE"), &globals::bone_index, globals::aim_lock_point, 5);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::TextDisabled(XOR("Aimbot Method"));
					ImGui::Combo(XOR("##PRIOR"), &globals::priority, globals::i_priority, 2);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::TextDisabled("Lock Method");
					ImGui::Combo("##LOCKONMETHOD", &globals::lockon, globals::lockonmethodss, 2);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::SetCursorPosX(25);
					render_SliderFloat("##AIM_SMOOTH", globals::aim_smooth, 200, 1.0f, 5.0f, customization::slider, "Smoothness: %0.1f");
					ImGui::Spacing();
					ImGui::Text("");
					ImGui::SetCursorPosX(25);
					render_SliderFloat("##AIM_DIST", globals::f_min_closest, 200, 1.0f, 1000.0f, customization::slider, "Aim Distance: %0.1fm");

					if (ImGui::IsItemHovered())
						ImGui::SetTooltip(XOR("Aim At Enemy Distance"));
					ImGui::EndChild();
				}

				colors[ImGuiCol_ChildBg] = previousCBG;
				ImGui::EndChild();
				break;
			case 1:
				colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.67f);
				style->ChildBorderSize = 0.f;
				ImGui::SetCursorPos(ImVec2(70, 40));
				if (ImGui::BeginChild("##visualchildmain", ImVec2(530, 410), true))
				{
					//nav bar pages { Main Visuals, ESP Customization , ESP Colors }
					ImGui::SetCursorPos(ImVec2(15, 10));
					if (ImGui::BeginChild("##visuals_pages", ImVec2(500, 45), true))
					{
						if (ImGui::Button("Main Visuals"))
						{
							previoussubTab = visualsubTab;
							visualsubTab = 0;
						}

						ImGui::SameLine();

						if (ImGui::Button("ESP Custimization"))
						{
							previoussubTab = visualsubTab;
							visualsubTab = 1;
						}

						ImGui::SameLine();

						if (ImGui::Button("ESP Colors"))
						{
							previoussubTab = visualsubTab;
							visualsubTab = 2;
						}
					}
					ImGui::EndChild();

					switch (visualsubTab)
					{
					case 0:
						ImGui::SetCursorPos(ImVec2(10, 60));
						if (ImGui::BeginChild("##VISUALSUBTAB1", ImVec2(247, 340), true))
						{
							ImGui::Checkbox(XOR("Show only Visible"), &globals::vis_only);
							ImGui::Checkbox(XOR("Show Team"), &globals::b_friendly);
							ImGui::Checkbox(XOR("Snap Lines"), &globals::b_line);
							ImGui::Checkbox(XOR("Skeletons"), &globals::b_skeleton);
							ImGui::Checkbox(XOR("Head Circle"), &globals::head_circle);
						}
						ImGui::EndChild();

						ImGui::SetCursorPos(ImVec2(275, 60));
						if (ImGui::BeginChild("##VISUALSUBTAB2", ImVec2(247, 340), true))
						{
							ImGui::TextDisabled("Player ESP");
							ImGui::Checkbox(XOR("Name"), &globals::b_namee);
							ImGui::Checkbox(XOR("HealthBar"), &globals::draw_health);
							ImGui::Checkbox(XOR("Distance"), &globals::distance);
							ImGui::Checkbox(XOR("Team ID"), &globals::team_id);

							ImGui::Spacing();

							ImGui::TextDisabled(XOR("Player ESP Boxes"));
							ImGui::Checkbox(XOR("2D Box"), &globals::b_box);
							ImGui::Checkbox(XOR("Diamond"), &globals::esp_box);
							ImGui::Checkbox(XOR("Box Fill"), &globals::box_bg);

						}
						ImGui::EndChild();
						break;
					case 1:
						ImGui::SetCursorPos(ImVec2(10, 60));
						if (ImGui::BeginChild("##VISUALSUBTAB3", ImVec2(247, 340), true))
						{
							ImGui::TextDisabled(XOR("Snap Line Style"));
							ImGui::PushItemWidth(200);
							ImGui::SliderInt(XOR("##LINE STYLE"), &globals::linetype, 1, 2);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XOR("1 = Bottom Screen\n2= Center Screen"));
							ImGui::NewLine();
							ImGui::SetCursorPosX(25);
							render_SliderFloat("##linethickness", globals::lines_thickness, 200, 1.f, 5.0f, customization::slider, "Line Thickness: %.1f");

							ImGui::Spacing();
							ImGui::NewLine();

							ImGui::TextDisabled(XOR("Crosshair"));

							ImGui::Checkbox(XOR("Show crosshair"), &globals::b_crosshair);
							ImGui::SameLine(215);
							ImGui::ColorEdit4(XOR("##crosshair Color4"), (float*)&color::crosshair, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
							if (ImGui::IsItemHovered())
								ImGui::SetTooltip(XOR("CrossHair Color"));
							ImGui::Combo(XOR("##crosshair type"), &globals::b_crosshair_type, globals::b_crosshairswitch, 5);

							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();

							ImGui::SetCursorPosX(20);
							render_SliderFloat("##crosshairthickness", globals::b_crosshair_thickness, 200, 1.f, 5.f, customization::slider, "Thickness: %.1f");

							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();

							ImGui::SetCursorPosX(20);
							render_SliderFloat("##crosshairsize", globals::b_crosshair_size, 200, 1.f, 30.f, customization::slider, "Size: %0.1f");
						}
						ImGui::EndChild();

						ImGui::SetCursorPos(ImVec2(275, 60));
						if (ImGui::BeginChild(XOR("##VISUALSUBTAB4"), ImVec2(247, 340), true))
						{
							ImGui::SetCursorPos(ImVec2(10, 10));
							render_SliderFloat("##maxdistanceesp", globals::max_distance, 200, 1.f, 1000.f, customization::slider, "Max Distance: %.1f");
							
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();

							ImGui::SetCursorPos(ImVec2(10, 40));
							render_SliderFloat("##boxthickness", globals::box_thickness, 200, 1.f, 5.f, customization::slider, "Box Thickness: %.1f");

							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();

							ImGui::SetCursorPos(ImVec2(10, 70));
							render_SliderFloat("##bonesthickness", globals::bones_thickness, 200, 1.f, 5.f, customization::slider, "Bone Thickness: %.1f");

							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();

							ImGui::SetCursorPos(ImVec2(10, 100));
							render_SliderFloat("##textespsize", globals::esp_text_size, 200, 1.f, 20, customization::slider, "ESP Text Size: %.1f");
						}
						ImGui::EndChild();
						break;
					case 2:
						ImGui::SetCursorPos(ImVec2(10, 60));
						ImGui::BeginChild(XOR("##colorchild0"), ImVec2(247, 340), true);
						ImGui::Text(XOR(""));
						ImGui::SameLine(125);
						ImGui::TextDisabled(XOR("Enemy"));
						ImGui::Separator();
						ImGui::Text(XOR("SnapLines"));
						ImGui::SameLine(125);
						ImGui::ColorEdit4(XOR("##V E lines"), (float*)&color::V_Line_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Enemy"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV E lines"), (float*)&color::NV_Line_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Enemy"));
						ImGui::Text(XOR("Skeletons"));
						ImGui::SameLine(125);
						ImGui::ColorEdit4(XOR("##V E bones"), (float*)&color::V_Bones_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Enemy"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV E bones"), (float*)&color::NV_Bones_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Enemy"));
						ImGui::Text(XOR("2D Box"));
						ImGui::SameLine(125);
						ImGui::ColorEdit4(XOR("##V E 2dboxes"), (float*)&color::V_2dBoxes_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Enemy"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV E 2dboxes"), (float*)&color::NV_2dBoxes_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Enemy"));
						ImGui::Text(XOR("Diamond"));
						ImGui::SameLine(125);
						ImGui::ColorEdit4(XOR("##V E 2ddboxes"), (float*)&color::V_2ddBoxes_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Enemy"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV E 2ddboxes"), (float*)&color::NV_2ddBoxes_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Enemy"));
						ImGui::Text(XOR("Box Fill"));
						ImGui::SameLine(125);
						ImGui::ColorEdit4(XOR("##V 2dboxes bg E"), (float*)&color::V_2dBoxes_BG_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Enemy"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV 2dboxes bg E"), (float*)&color::NV_2dBoxes_BG_Enemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Enemy"));
						ImGui::Text(XOR("Self Revive"));
						ImGui::SameLine(125);
						ImGui::ColorEdit4(XOR("##V SR E"), (float*)&color::V_Bones_SREnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Enemy"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV SR E"), (float*)&color::NV_Bones_SREnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Enemy"));
						ImGui::Text(XOR("Being Revived"));
						ImGui::SameLine(125);
						ImGui::ColorEdit4(XOR("##V PR E"), (float*)&color::V_Bones_BREnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Enemy"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV PR E"), (float*)&color::NV_Bones_BREnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Enemy"));
						ImGui::Text(XOR("Downed"));
						ImGui::SameLine(125);
						ImGui::ColorEdit4(XOR("##V D E"), (float*)&color::V_Bones_DEnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Enemy"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV D E"), (float*)&color::NV_Bones_DEnemy, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Enemy"));
						ImGui::EndChild();

						ImGui::SetCursorPos(ImVec2(275, 60));
						ImGui::BeginChild(XOR("##colorchild1"), ImVec2(247, 340), true);
						ImGui::TextDisabled(XOR("Team"));
						ImGui::Separator();
						ImGui::ColorEdit4(XOR("##V F lines"), (float*)&color::V_Line_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Friendly"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV F lines"), (float*)&color::NV_Line_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Friendly"));
						ImGui::ColorEdit4(XOR("##V F bones"), (float*)&color::V_Bones_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Friendly"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV F bones"), (float*)&color::NV_Bones_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Friendly"));
						ImGui::ColorEdit4(XOR("##V F 2dboxes"), (float*)&color::V_2dBoxes_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Friendly"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV F 2dboxes"), (float*)&color::NV_2dBoxes_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Friendly"));
						ImGui::ColorEdit4(XOR("##V F 2ddboxes"), (float*)&color::V_2ddBoxes_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Friendly"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV F 2ddboxes"), (float*)&color::NV_2ddBoxes_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Friendly"));
						ImGui::ColorEdit4(XOR("##V 2dboxes bg F"), (float*)&color::V_2dBoxes_BG_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Friendly"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV 2dboxes bg F"), (float*)&color::NV_2dBoxes_BG_F, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Friendly"));
						ImGui::ColorEdit4(XOR("##V SR F"), (float*)&color::V_Bones_SRF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Friendly"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV SR F"), (float*)&color::NV_Bones_SRF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Friendly"));
						ImGui::ColorEdit4(XOR("##V PR F"), (float*)&color::V_Bones_BRF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Friendly"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV PR F"), (float*)&color::NV_Bones_BRF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Friendly"));
						ImGui::ColorEdit4(XOR("##V D F"), (float*)&color::V_Bones_DF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Visible Enemy"));
						ImGui::SameLine();
						ImGui::ColorEdit4(XOR("##NV D F"), (float*)&color::NV_Bones_DF, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						if (ImGui::IsItemHovered())
							ImGui::SetTooltip(XOR("Not Visible Enemy"));
						ImGui::EndChild();
						break;
					}

				}

				colors[ImGuiCol_ChildBg] = previousCBG;
				ImGui::EndChild();
				break;
			case 2:
				colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.67f);
				style->ChildBorderSize = 0.f;
				ImGui::SetCursorPos(ImVec2(70, 40));
				if (ImGui::BeginChild("##colorschildmain", ImVec2(530, 410), true))
				{
					ImGui::SetCursorPos(ImVec2(10, 60));
					ImGui::BeginChild(XOR("##miscchild0"), ImVec2(247, 340), true);
					ImGui::Checkbox(XOR("No Recoil"), &globals::b_recoil);
					ImGui::Checkbox(XOR("Warning System"), &globals::warning_system);
					ImGui::NewLine();
					ImGui::SetCursorPosX(20);
					render_SliderFloat("##WARNINGDISTANCE", globals::warning_dist, 200, 1.f, 300.f, customization::slider, "Distance: %.1fm");
					//ImGui::Checkbox(XOR("FFA Mode"), &globals::ffa);
					//if (ImGui::IsItemHovered())
						//ImGui::SetTooltip(XOR("Your shooting at your ''team mates'' (enemies) change team colors for esp"));
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();

					ImGui::Checkbox(XOR("FOV Slider"), &globals::fov_slide);
					ImGui::NewLine();
					ImGui::SetCursorPosX(20);
					render_SliderFloat("##FOVSCALE", globals::fov_scale, 200, 0.f, 2.f, customization::slider, "FOV: %.1f");
				}
				ImGui::EndChild();

				ImGui::SetCursorPos(ImVec2(275, 60));
				if (ImGui::BeginChild("##miscchild", ImVec2(247, 340), true))
				{
					ImGui::Checkbox(XOR("Snow Fall"), &enableSnowfall);
					ImGui::Checkbox(XOR("Windows 7 AFK Circles"), &enableCircles);
				}
				ImGui::EndChild();

				colors[ImGuiCol_ChildBg] = previousCBG;
				ImGui::EndChild();
				break;
			case 3:
				colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.67f);
				style->ChildBorderSize = 0.f;
				ImGui::SetCursorPos(ImVec2(70, 40));
				if (ImGui::BeginChild("##miscchildmain", ImVec2(530, 410), true))
				{
					// Calculate cursor position relative to child window
					ImVec2 childContentRegionMax = ImGui::GetWindowContentRegionMax();
					float xPosition = childContentRegionMax.x * 0.5f - 10;
					float yPosition = ImGui::GetCursorPosY() + 5;

					static std::string str_warn = "Config Must be at least 3 char length!";

					// Set cursor position
					ImGui::SetCursorPos(ImVec2(xPosition, yPosition));
					ImGui::Text(XOR("Config"));

					int i_config = 0;

					ImGui::Spacing();
					ImGui::Spacing();

					// -->

					ImGui::Text("Profile 1 | F1");
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Press F1 to Load this config.");

					ImGui::SameLine(80);

					if (ImGui::Button(XOR("Save Profile1")))
					{
						std::string buf = XOR("Profile_1");

						if (!vec_files.empty() || vec_files.empty())
							Settings::Save_Settings(buf);
					}

					ImGui::SameLine();

					if (ImGui::Button("Delete Profile1"))
					{
						if (!vec_files.empty())
						{
							std::string str_config = "C:\\Configs\\Profile_1.ini";
							if (remove(str_config.c_str()) != 0)
							{
								str_warn = "Could not delete!";
								ImGui::OpenPopup("warn");
							}
							str_config = "";
							Settings::str_config_name = "";
						}
					}
					//<--
					ImGui::NewLine();
					//-->
					ImGui::Text("Profile 2 | F2");
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Press F2 to Load this config.");

					ImGui::SameLine(80);

					if (ImGui::Button(XOR("Save Profile2")))
					{
						std::string profile2 = XOR("Profile_2");

						if (!vec_files.empty() || vec_files.empty())
							Settings::Save_Settings(profile2);
					}

					ImGui::SameLine();

					if (ImGui::Button("Delete Profile2"))
					{
						if (!vec_files.empty())
						{
							std::string str_config = "C:\\Configs\\Profile_2.ini";
							if (remove(str_config.c_str()) != 0)
							{
								str_warn = "Could not delete!";
								ImGui::OpenPopup("warn");
							}
							str_config = "";
							Settings::str_config_name = "";
						}
					}
					//<--
					ImGui::NewLine();
					//-->
					ImGui::Text("Profile 3 | F3");
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Press F3 to Load this config.");

					ImGui::SameLine(80);

					if (ImGui::Button(XOR("Save Profile3")))
					{
						std::string buf2 = XOR("Profile_3");

						if (!vec_files.empty() || vec_files.empty())
							Settings::Save_Settings(buf2);
					}

					ImGui::SameLine();

					if (ImGui::Button("Delete Profile3"))
					{
						if (!vec_files.empty())
						{
							std::string str_config = "C:\\Configs\\Profile_3.ini";
							if (remove(str_config.c_str()) != 0)
							{
								str_warn = "Could not delete!";
								ImGui::OpenPopup("warn");
							}
							str_config = "";
							Settings::str_config_name = "";
						}
					}
					//<--
				}

				colors[ImGuiCol_ChildBg] = previousCBG;
				ImGui::EndChild();
				break;
				case 4:
					colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.12f, 0.12f, 0.67f);
					style->ChildBorderSize = 0.f;
					ImGui::SetCursorPos(ImVec2(70, 40));
					if (ImGui::BeginChild("##miscchildmain", ImVec2(530, 410), true))
					{
						ImGui::Text(XOR("Text"));
						ImGui::SameLine(140);
						ImGui::ColorEdit4(XOR("##Text"), (float*)&customization::text, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

						ImGui::SameLine(180);
						ImGui::Text(XOR("Button"));
						ImGui::SameLine(270);
						ImGui::ColorEdit4(XOR("##BUTTON"), (float*)&customization::button, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

						ImGui::SameLine(420);
						if (ImGui::Button("Default Theme"))
						{
							customization::text = ImColor{ 255, 255, 255, 255 };
							customization::text_disabled = ImColor{ 128, 128, 128, 255 };
							customization::windowbg = ImColor{ 23, 18, 18, 255 };
							customization::popup = ImColor{ 20, 20, 20, 240 };
							customization::border = ImColor{ 255, 0, 0, 128 };
							customization::framebg = ImColor{ 49, 49, 49, 138 };
							customization::framebgh = ImColor{ 255, 84, 84, 102 };
							customization::framebga = ImColor{ 171, 15, 15, 171 };
							customization::checkmark = ImColor{ 255, 5, 5, 255 };
							customization::button = ImColor{ 31, 31, 31, 107 };
							customization::buttonh = ImColor{ 255, 84, 84, 102 };
							customization::buttona = ImColor{ 171, 15, 15, 171 };
							customization::slider = ImColor{ 255, 0, 0, 255 };
							customization::header = ImColor{ 92, 88, 88, 255 };
							customization::headerh = ImColor{ 255, 0, 0, 255 };
							customization::headera = ImColor{ 0, 255, 0, 255 };
							customization::wbs = 5.f;
							customization::wr = 0.f;
						}

						ImGui::Text(XOR("Text Disabled"));
						ImGui::SameLine(140);
						ImGui::ColorEdit4(XOR("##TextDisabled"), (float*)&customization::text_disabled, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						
						ImGui::SameLine(180);
						ImGui::Text(XOR("Button Hover"));
						ImGui::SameLine(270);
						ImGui::ColorEdit4(XOR("##BUTTONHOVER"), (float*)&customization::buttonh, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

						ImGui::SameLine(420);
						if (ImGui::Button("Purple Theme"))
						{
							customization::text = ImColor{ 255, 255, 255, 255 };
							customization::text_disabled = ImColor{ 128, 128, 128, 255 };
							customization::windowbg = ImColor{ 23, 18, 18, 255 };
							customization::popup = ImColor{ 20, 20, 20, 240 };
							customization::border = ImColor{ 215, 21, 235, 128 };
							customization::framebg = ImColor{ 49, 49, 49, 138 };
							customization::framebgh = ImColor{ 170, 15, 223, 102 };
							customization::framebga = ImColor{ 232, 18, 235, 171 };
							customization::checkmark = ImColor{ 184, 14, 249, 255 };
							customization::button = ImColor{ 31, 31, 31, 107 };
							customization::buttonh = ImColor{ 166, 43, 221, 103 };
							customization::buttona = ImColor{ 232, 18, 235, 171 };
							customization::slider = ImColor{ 167, 11, 228, 255 };
							customization::header = ImColor{ 92, 88, 88, 255 };
							customization::headerh = ImColor{ 166, 43, 221, 103 };
							customization::headera = ImColor{ 232, 18, 235, 171 };
							customization::wbs = 3.6f;
							customization::wr = 0.f;
						}

						ImGui::Text(XOR("Window Background"));
						ImGui::SameLine(140);
						ImGui::ColorEdit4(XOR("##WindowBackground"), (float*)&customization::windowbg, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						
						ImGui::SameLine(180);
						ImGui::Text(XOR("Button Active"));
						ImGui::SameLine(270);
						ImGui::ColorEdit4(XOR("##BUTTONACTIVE"), (float*)&customization::buttona, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

						ImGui::Text(XOR("Popup Background"));
						ImGui::SameLine(140);
						ImGui::ColorEdit4(XOR("##PopUPBackground"), (float*)&customization::popup, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						
						ImGui::SameLine(180);
						ImGui::Text(XOR("Slider"));
						ImGui::SameLine(270);
						ImGui::ColorEdit4(XOR("##SLIDERFOAD"), (float*)&customization::slider, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

						ImGui::Text(XOR("Border"));
						ImGui::SameLine(140);
						ImGui::ColorEdit4(XOR("##Border"), (float*)&customization::border, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						
						ImGui::SameLine(180);
						ImGui::Text(XOR("Header"));
						ImGui::SameLine(270);
						ImGui::ColorEdit4(XOR("##headerr"), (float*)&customization::header, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

						ImGui::Text(XOR("Frame Background"));
						ImGui::SameLine(140);
						ImGui::ColorEdit4(XOR("##FrameBackground"), (float*)&customization::framebg, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
					
						ImGui::SameLine(180);
						ImGui::Text(XOR("Header Hovered"));
						ImGui::SameLine(270);
						ImGui::ColorEdit4(XOR("##headerrh"), (float*)&customization::headerh, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

						ImGui::Text(XOR("Frame Background Hover"));
						ImGui::SameLine(140);
						ImGui::ColorEdit4(XOR("##FrameBackgroundHover"), (float*)&customization::framebgh, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						
						ImGui::SameLine(180);
						ImGui::Text(XOR("Header Active"));
						ImGui::SameLine(270);
						ImGui::ColorEdit4(XOR("##headerra"), (float*)&customization::headera, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);

						ImGui::Text(XOR("Frame Background Active"));
						ImGui::SameLine(140);
						ImGui::ColorEdit4(XOR("##FrameBackgroundActive"), (float*)&customization::framebga, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						
						ImGui::Text(XOR("CheckMark"));
						ImGui::SameLine(140);
						ImGui::ColorEdit4(XOR("##CheckMark"), (float*)&customization::checkmark, ImGuiColorEditFlags_::ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_::ImGuiColorEditFlags_AlphaBar);
						
						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();

						render_SliderFloat("##WindowBordersize", customization::wbs, 125, 0, 20.0f, customization::slider, "Window Border Size: %.1f");

						ImGui::Spacing();
						ImGui::Spacing();
						ImGui::Spacing();

						render_SliderFloat("##windowrounding", customization::wr, 125, 0, 20.0f, customization::slider, "Window Rounding: %.1f");

						ImGui::Spacing();
					}

					colors[ImGuiCol_ChildBg] = previousCBG;
					ImGui::EndChild();
					break;
			}

			//ImGui::SetCursorPos(ImVec2(10, 10));
			//ImGui::Text("Controller Connected: %s", aimbot::isControllerConnected() ? "Yes" : "No");
			
			ImGui::SetCursorPos(ImVec2(475, 10));
			EditorColorScheme::MySwitch(XOR("Cheats Enabled"), &globals::menu_available);
			if (ImGui::IsItemHovered())
				ImGui::SetTooltip(XOR("Self Explanatory Hit End on keyboard to enable/disable Cheats"));

			ImGui::End();
		}

	}
}