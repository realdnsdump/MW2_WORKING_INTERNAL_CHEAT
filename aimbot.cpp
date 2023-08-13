#include "game.h"
#include "globals.h"
#include "syscall.h"
#include "esp.h"
#include <cfloat>
#include <d3d9.h>

namespace aimbot
{
	bool once = false;
	bool bUpdateTick = false;
	const DWORD nTickTime = 64;//64 ms
	constexpr float init_center_dist = 2750.0f;
	float min_center_dist = init_center_dist;
	float init_distance = 1000.0f;
	float min_distance = init_distance;
	Vector3 header_to_bladder[6], right_foot_to_bladder[5], left_foot_to_bladder[5], right_hand[5], left_hand[5];
	Vector2 lock_position;

	struct velocityInfo_t
	{
		Vector3 lastPos;
		Vector3 delta;
	};

	std::map<DWORD, velocityInfo_t> velocityMap;

	int local_index()
	{
		if (globals::platform == 0)
		{
			auto client_info = bnet::get_client_info();
			if (is_valid_ptr(client_info))
			{
				auto local_index = *(uintptr_t*)(client_info + bnet::local_index);
				if (is_bad_ptr(local_index)) return 0;

				return *(int*)(local_index + bnet::local_index_pos);
			}
			return 0;
		}
		else if (globals::platform == 1)
		{
			auto client_info = steam::Get_cg_t();
			if (is_valid_ptr(client_info))
			{
				auto local_index = *(uintptr_t*)(client_info + steam::local_index);
				if (is_bad_ptr(local_index)) return 0;

				return *(int*)(local_index + steam::local_index_pos);
			}
			return 0;
		}
	}

	void reset_lock()
	{
		min_center_dist = init_center_dist;
		min_distance = init_distance;
	}

	__forceinline bool is_key_down(int vk_key)
	{
		return ((GetAsyncKeyState(vk_key) & 0x8000) ? 1 : 0);
	}

	unsigned long get_bone_opt()
	{
		switch (globals::bone_index)
		{
		case 0:
			return sdk::BONE_INDEX::BONE_POS_HEAD;
		case 1:
			return sdk::BONE_INDEX::BONE_POS_NECK;
		case 2:
			return sdk::BONE_INDEX::BONE_POS_CHEST;
		case 3:
			return sdk::BONE_INDEX::BONE_POS_PELVIS;
		};
		return sdk::BONE_INDEX::BONE_POS_HEAD;
	}

	class Result
	{
	public:
		bool hasResult;
		float a;
		float b;
	};

	void start_tick()
	{
		static DWORD lastTick = 0;
		DWORD t = GetTickCount();
		bUpdateTick = lastTick < t;

		if (bUpdateTick)
			lastTick = t + nTickTime;
	}


	void update_vel_map(int index, Vector3 vPos)
	{
		if (!bUpdateTick)
			return;

		velocityMap[index].delta = vPos - velocityMap[index].lastPos;
		velocityMap[index].lastPos = vPos;
	}

	void clear_map()
	{
		if (!velocityMap.empty()) { velocityMap.clear(); }
	}

	Vector3 get_speed(int index)
	{
		return velocityMap[index].delta;
	}

	Result MidnightSolver(float a, float b, float c)
	{
		Result res;

		double subsquare = b * b - 4 * a * c;

		if (subsquare < 0)
		{
			res.hasResult = false;
			return res;
		}
		else
		{
			res.hasResult = true,
				res.a = (float)((-b + sqrt(subsquare)) / (2 * a));
			res.b = (float)((-b - sqrt(subsquare)) / (2 * a));
		}
		return res;
	}

	Vector3 prediction_solver(Vector3 local_pos, Vector3 position, int index, float bullet_speed)
	{
		Vector3 aimPosition = Vector3().Zero();
		auto target_speed = get_speed(index);

		local_pos -= position;

		float a = (target_speed.x * target_speed.x) + (target_speed.y * target_speed.y) + (target_speed.z * target_speed.z) - ((bullet_speed * bullet_speed) * 100);
		float b = (-2 * local_pos.x * target_speed.x) + (-2 * local_pos.y * target_speed.y) + (-2 * local_pos.z * target_speed.z);
		float c = (local_pos.x * local_pos.x) + (local_pos.y * local_pos.y) + (local_pos.z * local_pos.z);

		local_pos += position;

		Result r = MidnightSolver(a, b, c);

		if (r.a >= 0 && !(r.b >= 0 && r.b < r.a))
		{
			aimPosition = position + target_speed * r.a;
		}
		else if (r.b >= 0)
		{
			aimPosition = position + target_speed * r.b;
		}

		return aimPosition;

	}

	void lock_on_target()
	{
		if (min_center_dist == init_center_dist && min_distance == init_distance)
			return;

		float target_x = 0;
		float target_y = 0;
		const float center_x = g_game::e_data::refdef->Width / 2;
		const float center_y = g_game::e_data::refdef->Height / 2;

		if (lock_position.x > center_x)
		{
			target_x = -(center_x - lock_position.x);
			target_x /= globals::aim_smooth;
			if (target_x + center_x > center_x * 2)
				target_x = 0;
		}
		else if (lock_position.x < center_x)
		{
			target_x = lock_position.x - center_x;
			target_x /= globals::aim_smooth;
			if (target_x + center_x < 0)
				target_x = 0;
		}

		if (lock_position.y > center_y)
		{
			target_y = -(center_y - lock_position.y);
			target_y /= globals::aim_smooth;
			if (target_y + center_y > center_y * 2)
				target_y = 0;
		}
		else if (lock_position.y < center_y)
		{
			target_y = lock_position.y - center_y;
			target_y /= globals::aim_smooth;
			if (target_y + center_y < 0)
				target_y = 0;
		}

		// Randomize x and y offsets
		const float max_offset = 0.01f;
		const float random_x_offset = (2.0f * max_offset * static_cast<float>(rand()) / RAND_MAX) - max_offset;
		const float random_y_offset = (2.0f * max_offset * static_cast<float>(rand()) / RAND_MAX) - max_offset;

		target_x += random_x_offset;
		target_y += random_y_offset;

		if (target_x != 0 || target_y != 0)
			sysingthecall::send_mouse_input(MOUSEEVENTF_MOVE, static_cast<DWORD>(target_x), static_cast<DWORD>(target_y), NULL, NULL);
	}

	void is_aiming()
	{
		if (globals::lockon == 0) //weapon ads
		{
			if (is_key_down(VK_RBUTTON))
			{
				globals::is_aiming = true;
				lock_on_target();
			}
			globals::is_aiming = false;
		}

		if (globals::lockon == 1) //weapon fire
		{
			if (is_key_down(VK_LBUTTON))
			{
				globals::is_aiming = true;
				lock_on_target();
			}
			globals::is_aiming = false;
		}
	}

	bool check_valid_bones(unsigned long i)
	{
		if (sdk::get_bone_by_player_index(i, sdk::BONE_POS_HEAD, &header_to_bladder[0]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_NECK, &header_to_bladder[1]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_CHEST, &header_to_bladder[2]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_MID, &header_to_bladder[3]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_TUMMY, &header_to_bladder[4]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_PELVIS, &header_to_bladder[5]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_1, &right_foot_to_bladder[1]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_2, &right_foot_to_bladder[2]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_3, &right_foot_to_bladder[3]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_FOOT_4, &right_foot_to_bladder[4]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_1, &left_foot_to_bladder[1]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_2, &left_foot_to_bladder[2]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_3, &left_foot_to_bladder[3]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_FOOT_4, &left_foot_to_bladder[4]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_1, &right_hand[1]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_2, &right_hand[2]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_3, &right_hand[3]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_LEFT_HAND_4, &right_hand[4]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_1, &left_hand[1]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_2, &left_hand[2]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_3, &left_hand[3]) &&
			sdk::get_bone_by_player_index(i, sdk::BONE_POS_RIGHT_HAND_4, &left_hand[4]))
		{
			return true;
		}
		else { return false; }return true;
	}

}