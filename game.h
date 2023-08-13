#pragma once
#include "stdafx.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "sdk.h"

namespace g_game
{
	float units_to_m(float units);
	bool is_visible(int entityNum);
	void aimbot(Vector2 bone_screen_pos, float fdistance, float healthPercentage, bool visible);

	namespace e_data
	{
		extern sdk::refdef_t* refdef;
		extern sdk::refdef_t reefdeef;
	}
}
