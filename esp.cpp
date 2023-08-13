#include "game.h"
#include "globals.h"
#include "aimbot.h"

namespace esp
{
#define M_PI   3.141592654f

	bool w2s(Vector3 world_location, Vector2* ScreenPos)
	{
		if (sdk::in_game())
		{
			if (globals::platform == 0)
			{
				auto Camera = *reinterpret_cast<uintptr_t*>(g_data::base + bnet::camera_ptr);
				if (!Camera)
					return false;

				auto CameraPos = *reinterpret_cast<Vector3*>(Camera + 0x1F8);
				if (CameraPos.IsZero())
					return false;

				auto local = world_location - CameraPos;
				auto trans = Vector3{
					local.Dot(g_game::e_data::refdef->view.axis[1]),
					local.Dot(g_game::e_data::refdef->view.axis[2]),
					local.Dot(g_game::e_data::refdef->view.axis[0])
				};

				if (trans.z < 0.01f) {
					return false;
				}

				ScreenPos->x = (static_cast<float>(g_game::e_data::refdef->Width) / 2) * (1 - (trans.x / g_game::e_data::refdef->view.tan_half_fov.x / trans.z));
				ScreenPos->y = (static_cast<float>(g_game::e_data::refdef->Height) / 2) * (1 - (trans.y / g_game::e_data::refdef->view.tan_half_fov.y / trans.z));

				//if (ScreenPos->x < 1 || ScreenPos->y < 1 || (ScreenPos->x > g_game::e_data::refdef->Width) || (ScreenPos->y > g_game::e_data::refdef->Height)) {
				//	return false;
				//}

				return true;
			}
			else if (globals::platform == 1)
			{
				auto Camera = *reinterpret_cast<uintptr_t*>(g_data::base + steam::camera_ptr);
				if (!Camera)
					return false;

				auto CameraPos = *reinterpret_cast<Vector3*>(Camera + 0x1F8);
				if (CameraPos.IsZero())
					return false;

				auto local = world_location - CameraPos;
				auto trans = Vector3{
					local.Dot(g_game::e_data::refdef->view.axis[1]),
					local.Dot(g_game::e_data::refdef->view.axis[2]),
					local.Dot(g_game::e_data::refdef->view.axis[0])
				};

				if (trans.z < 0.01f) {
					return false;
				}

				ScreenPos->x = (static_cast<float>(g_game::e_data::refdef->Width) / 2) * (1 - (trans.x / g_game::e_data::refdef->view.tan_half_fov.x / trans.z));
				ScreenPos->y = (static_cast<float>(g_game::e_data::refdef->Height) / 2) * (1 - (trans.y / g_game::e_data::refdef->view.tan_half_fov.y / trans.z));

				//if (ScreenPos->x < 1 || ScreenPos->y < 1 || (ScreenPos->x > g_game::e_data::refdef->Width) || (ScreenPos->y > g_game::e_data::refdef->Height)) {
				//	return false;
				//}

				return true;
			}
		}
	}

	bool wts(Vector3& W_POS, Vector2* S_POS)
	{
		return w2s(W_POS, S_POS);
	}

	namespace g_draw
	{
		void draw_line(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
		{
			auto draw = ImGui::GetBackgroundDrawList();
			draw->AddLine(from, to, color, thickness);

		}

		void draw_circle(const ImVec2& position, float radius, uint32_t color, float thickness)
		{
			constexpr double M_PI1 = 3.14159265358979323846;
			float step = (float)M_PI1 * 2.0f / thickness;
			for (float a = 0; a < (M_PI * 2.0f); a += step)
			{
				auto draw = ImGui::GetBackgroundDrawList();
				draw->AddLine(
					ImVec2(radius * cosf(a) + position.x, radius * sinf(a) + position.y),
					ImVec2(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y),
					color,
					1.5f
				);
			}
		}

		void draw_sketch_edge_text(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, uint32_t color, bool center, uint32_t EdgeColor = 0xFF000000)
		{
			constexpr float fStrokeVal1 = 1.0f;
			ImGuiWindow* window = ImGui::GetCurrentWindow();

			float Edge_a = (EdgeColor >> 24) & 0xff;
			float Edge_r = (EdgeColor >> 16) & 0xff;
			float Edge_g = (EdgeColor >> 8) & 0xff;
			float Edge_b = (EdgeColor) & 0xff;
			std::stringstream steam(text);
			std::string line;
			float y = 0.0f;
			int i = 0;
			while (std::getline(steam, line))
			{
				ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());
				if (center)
				{
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + fStrokeVal1, pos.y + textSize.y * i), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), color, line.c_str());
				}
				else
				{
					window->DrawList->AddText(pFont, size, ImVec2((pos.x) - fStrokeVal1, (pos.y + textSize.y * i)), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x) + fStrokeVal1, (pos.y + textSize.y * i)), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x), (pos.y + textSize.y * i) - fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2((pos.x), (pos.y + textSize.y * i) + fStrokeVal1), ImGui::GetColorU32(ImVec4(Edge_r / 255, Edge_g / 255, Edge_b / 255, Edge_a / 255)), line.c_str());
					window->DrawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), color, line.c_str());
				}
				y = pos.y + textSize.y * (i + 1);
				i++;
			}
		}

		void draw_crosshair()
		{
			ImVec2 center = ImVec2(g_game::e_data::refdef->Width / 2, g_game::e_data::refdef->Height / 2);

			g_draw::draw_line(ImVec2((center.x), (center.y) - globals::b_crosshair_size), ImVec2((center.x), (center.y) + globals::b_crosshair_size), color::crosshair, globals::b_crosshair_thickness);
			g_draw::draw_line(ImVec2((center.x) - globals::b_crosshair_size, (center.y)), ImVec2((center.x) + globals::b_crosshair_size, (center.y)), color::crosshair, globals::b_crosshair_thickness);
		}

		void draw_fov(const float aimbot_fov)
		{
			ImVec2 center = ImVec2(g_game::e_data::refdef->Width / 2, g_game::e_data::refdef->Height / 2);

			g_draw::draw_circle(center, aimbot_fov, color::showfov, 200.0f);
		}
	}

	float Distance3D(Vector3 point1, Vector3 point2)
	{
		float distance = sqrt((point1.x - point2.x) * (point1.x - point2.x) +
			(point1.y - point2.y) * (point1.y - point2.y) +
			(point1.z - point2.z) * (point1.z - point2.z));
		return distance;
	}

	void d_bones(Vector3 from, Vector3 to, Vector3 dpos, ImColor color)
	{
		auto draw = ImGui::GetBackgroundDrawList();

		if (Distance3D(dpos, from) > 118)
			return;
		if (Distance3D(dpos, to) > 118)
			return;
		if (Distance3D(from, to) > 39)
			return;

		Vector2 W2S_FROM;
		if (!esp::wts(from, &W2S_FROM))
			return;

		Vector2 W2S_TO;
		if (!esp::wts(to, &W2S_TO))
			return;

		draw->AddLine(ImVec2(W2S_FROM.x, W2S_FROM.y), ImVec2(W2S_TO.x, W2S_TO.y), color, globals::bones_thickness);
	}
}