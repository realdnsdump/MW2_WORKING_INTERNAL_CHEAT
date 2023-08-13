#include "globals.h"
#include "sdk.h"

namespace globals
{
	//BNET OR STEAM
	//<--     --> 
	int platform;
	const char* platform_option[] = { "Battle.Net", "Steam" };

	bool menu_available = true;
	bool b_menu_open = true;

	//ESP
	bool b_visible = false;
	bool vis_only = false;
	bool b_crosshair = false;
	int b_crosshair_type = 0;
	float b_crosshair_thickness = 1.8f;
	float b_crosshair_size = 8.0f;
	const char* b_crosshairswitch[] = { "Cross", "dot", "Circle", "Smiley Face", "diamond"};
	bool esp_box = false;
	bool b_box = false;
	float box_thickness = 1.0;
	float bones_thickness = 1.5;
	bool box_bg = false;
	bool b_line = false;
	float lines_thickness = 1.0f;
	int linetype = 1;
	bool b_skeleton = false;
	bool head_circle = false;
	bool b_namee = false;
	bool distance = false;
	bool draw_health = false;
	bool team_id = false;
	bool b_friendly = false;
	float max_distance = 500.0f;
	float esp_text_size = 15.0f;

	//AIMBOT
	bool b_lock = false;
	bool disable_bd = false;
	bool self_revive = false;
	bool b_revive = false;
	bool bone_scan = false;
	bool smart_bone = false;
	bool prediction = false;
	bool b_recoil = false;
	float aim_smooth = 1;
	int priority;
	const char* i_priority[] = { "Smart Crosshair", "Smart FOV Circle"};
	int lockon;
	const char* lockonmethodss[] = { "Weapon ADS", "Weapon Fire" };
	float f_min_closest = 100.f;
	bool b_fov = false;
	float f_fov_size = 90.0f;
	bool b_skip_knocked = false;
	int bone_index = 0;
	const char* aim_lock_point[] = { "Head", "Neck", "Chest", "Spine", "Random"};

	//MISC
	bool warning_system = false;
	float warning_dist = 50.0f;
	bool fov_slide = false;
	float fov_scale;
	int mpwzloudout;
	const char* mpwzloudouts[] = { "MP", "WZ" };
	int classnumber;
	const char* classnumbers[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

	bool enableSnowfall = false;
	bool enableCircles = false;
	bool gameBorder = false;
	bool rgbGB = false;

	bool enabled = true;

	bool b_in_game = false;
	bool local_is_alive = false;
	bool is_aiming = false;

	int max_player_count = 0;
	int connecte_players = 0;
	int local_team_id;
	int call = 0;
	int player_index;

	float bullet_speed = 2250.5f;

	uintptr_t local_ptr;
	Vector3 local_pos;
	uintptr_t refdef_ptr;
	sdk::refdef_t* refdefadd;
	Vector3 view_angles;
}

namespace color
{
	ImColor Color{ 255,255,255,255 };
	ImColor V_2dBoxes_Enemy{ 26, 214, 32, 255 };
	ImColor V_2dBoxes_F{ 26, 214, 32, 255 };
	ImColor NV_2dBoxes_Enemy{ 255, 0, 0, 255 };
	ImColor NV_2dBoxes_F{ 255, 0, 0, 255 };

	ImColor V_2ddBoxes_Enemy{ 26, 214, 32, 255 };
	ImColor V_2ddBoxes_F{ 26, 214, 32, 255 };
	ImColor NV_2ddBoxes_Enemy{ 255, 0, 0, 255 };
	ImColor NV_2ddBoxes_F{ 255, 0, 0, 255 };

	ImColor V_2dBoxes_BG_Enemy{ 128, 128, 128, 128 };
	ImColor V_2dBoxes_BG_F{ 128, 128, 128, 128 };
	ImColor NV_2dBoxes_BG_Enemy{ 128, 128, 128, 128 };
	ImColor NV_2dBoxes_BG_F{ 128, 128, 128, 128 };

	ImColor V_Bones_Enemy{ 26, 214, 32, 255 };
	ImColor V_Bones_F{ 26, 214, 32, 255 };
	ImColor NV_Bones_Enemy{ 255, 0, 0, 255 };
	ImColor NV_Bones_F{ 255, 0, 0, 255 };

	ImColor V_Bones_DEnemy{ 66, 227, 245, 255 };
	ImColor V_Bones_DF{ 26, 214, 32, 255 };
	ImColor NV_Bones_DEnemy{ 66, 227, 245, 255 };
	ImColor NV_Bones_DF{ 255, 0, 0, 255 };

	ImColor V_Bones_SREnemy{ 226, 252, 93, 255 };
	ImColor V_Bones_SRF{ 26, 214, 32, 255 };
	ImColor NV_Bones_SREnemy{ 226, 252, 93, 255 };
	ImColor NV_Bones_SRF{ 255, 0, 0, 255 };

	ImColor V_Bones_BREnemy{ 245, 42, 228, 255 };
	ImColor V_Bones_BRF{ 26, 214, 32, 255 };
	ImColor NV_Bones_BREnemy{ 245, 42, 228, 255 };
	ImColor NV_Bones_BRF{ 255, 0, 0, 255 };

	ImColor V_Line_Enemy{ 26, 214, 32, 255 };
	ImColor V_Line_F{ 26, 214, 32, 255 };
	ImColor NV_Line_Enemy{ 255, 0, 0, 255 };
	ImColor NV_Line_F{ 255, 0, 0, 255 };

	ImColor V_Friendly{ 14, 244, 217, 255 };
	ImColor NV_Friendly{ 240, 12, 249, 255 };

	ImColor V_Boxes{ 26, 214, 32, 255 };
	ImColor NV_Boxes{ 255, 0, 0, 255 };

	ImColor V_Bones{ 26, 214, 32, 255 };
	ImColor NV_Bones{ 255, 0, 0, 255 };

	ImColor V_Line{ 26, 214, 32, 255 };
	ImColor NV_Line{ 255, 0, 0, 255 };

	ImColor crosshair{ 255, 255, 255, 255 };
	ImColor showfov{ 255, 255, 255, 255 };

	ImColor radar_boarder{ 255, 0, 0, 255 };
	ImColor radar_bg{ 0, 0, 0, 60 };

}

namespace screenshot
{
	 bool visuals = true;
	 bool* pDrawEnabled = nullptr;
	 uint32_t screenshot_counter = 0;
	 uint32_t  bit_blt_log = 0;
	 const char* bit_blt_fail;
	 uintptr_t  bit_blt_anotherlog;
	 uint32_t	GdiStretchBlt_log = 0;
	 const char* GdiStretchBlt_fail;
	 uintptr_t  GdiStretchBlt_anotherlog;
	 uintptr_t	texture_copy_log = 0;
	 uintptr_t virtualqueryaddr = 0;
}

namespace customization
{
	//colors
	ImColor text{ 255, 255, 255, 255 };
	ImColor text_disabled{ 128, 128, 128, 255 };
	ImColor windowbg{ 23, 18, 18, 255 };
	ImColor popup{ 20, 20, 20, 240 };
	ImColor Gameborder{ 255, 0, 0, 128 };
	ImColor border{ 255, 0, 0, 128 };
	ImColor framebg{ 49, 49, 49, 138 };
	ImColor framebgh{ 255, 84, 84, 102 };
	ImColor framebga{ 171, 15, 15, 171 };
	ImColor checkmark{ 255, 5, 5, 255 };
	ImColor button{ 31, 31, 31, 107 };
	ImColor buttonh{ 255, 84, 84, 102 };
	ImColor buttona{ 171, 15, 15, 171 };
	ImColor slider{ 255, 0, 0, 255 };
	ImColor header{ 92, 88, 88, 255 };
	ImColor headerh{ 255, 0, 0, 255 };
	ImColor headera{ 0, 255, 0, 255 };

	//placement ?? 
	float wbs = 5.f;
	float wr = 0.f;

}