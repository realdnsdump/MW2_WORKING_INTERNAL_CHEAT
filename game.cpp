#include "stdafx.h"
#include "game.h"
#include "sdk.h"
#include "globals.h"
#include <stdarg.h>
#include <random>
#include <chrono>
#include <bitset>
#include <vector>
#include "defs.h"
#include "aimbot.h"
#include "esp.h"
#include <array>
#include <string.h>

bool once = false;

namespace g_game
{
	float units_to_m(float units) {
		return units * 0.0254;
	}

	bool is_visible(int entityNum) {
		if (globals::platform == 0)
		{
			auto client_info = bnet::get_client_info();
			if (!client_info || bnet::visible == 0x0)
				return false;

			using ClientBits = std::array<unsigned int, 7>;
			auto& sightedEnemyFools = *(ClientBits*)(client_info + bnet::visible);

			auto bitmask = 0x80000000 >> (entityNum & 0x1F);
			return sightedEnemyFools[entityNum >> 5] & bitmask;
		}
		else if (globals::platform == 1)
		{
			auto client_info = steam::Get_cg_t();
			if (!client_info || steam::visible == 0x0)
				return false;

			using ClientBits = std::array<unsigned int, 7>;
			auto& sightedEnemyFools = *(ClientBits*)(client_info + steam::visible);

			auto bitmask = 0x80000000 >> (entityNum & 0x1F);
			return sightedEnemyFools[entityNum >> 5] & bitmask;
		}

	}

	void aimbot(Vector2 bone_screen_pos, float fdistance, float healthPercentage, bool visible)
	{
		const float x = bone_screen_pos.x - (g_game::e_data::refdef->Width / 2);
		const float y = bone_screen_pos.y - (g_game::e_data::refdef->Height / 2);
		const float center_dis = x * x + y * y;
		float min_health = FLT_MAX;

		const bool is_closer_target = fdistance < aimbot::min_distance;
		const bool is_lower_health_target = healthPercentage < min_health;

		switch (globals::priority)
		{
		case 0:
			if (center_dis < aimbot::min_center_dist)
			{
				if (is_lower_health_target || (is_closer_target && !is_lower_health_target))
				{
					min_health = healthPercentage;
					aimbot::min_distance = fdistance;
					aimbot::min_center_dist = center_dis;
					aimbot::lock_position = bone_screen_pos;
				}
				else

					aimbot::lock_position = Vector2(0, 0);
			}
			break;
		case 1:
			if (center_dis < pow(globals::f_fov_size, 2))
			{
				if (is_lower_health_target || (is_closer_target && !is_lower_health_target))
				{
					min_health = healthPercentage;
					aimbot::min_distance = fdistance;
					aimbot::min_center_dist = center_dis;
					aimbot::lock_position = bone_screen_pos;
				}
				else
					aimbot::lock_position = Vector2(0, 0);
			}
			break;
		}
	}

	namespace e_data
	{
		sdk::refdef_t* refdef;
		sdk::refdef_t reefdeef;
	}
}