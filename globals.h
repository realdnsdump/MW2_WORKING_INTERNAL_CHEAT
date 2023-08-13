#pragma once
#include "stdafx.h"
#include "sdk.h"
#include <vector>
#include <map>
#include <string>
#include <filesystem>
#include <Windows.h>
#include <iostream>
#include <shlobj.h>
#include <fstream>
#include <vector>
#include <sstream>

namespace globals
{
	//BNET OR STEAM
	//<--     --> 
	extern int platform;
	extern const char* platform_option[];

	extern bool menu_available;
	extern bool b_menu_open;

	//ESP
	extern bool b_visible;
	extern bool vis_only;
	extern bool b_crosshair;
	extern int b_crosshair_type;
	extern float b_crosshair_thickness;
	extern float b_crosshair_size;
	extern const char* b_crosshairswitch[];
	extern bool esp_box;
	extern bool b_box;
	extern float box_thickness;
	extern float bones_thickness;
	extern bool box_bg;
	extern bool b_line;
	extern float lines_thickness;
	extern int linetype;
	extern bool b_skeleton;
	extern bool head_circle;
	extern bool b_namee;
	extern bool distance;
	extern bool draw_health;
	extern bool team_id;
	extern bool b_friendly;
	extern float max_distance;
	extern float esp_text_size;

	//AIMBOT
	extern bool b_lock;
	extern bool disable_bd;
	extern bool self_revive;
	extern bool b_revive;
	extern bool bone_scan;
	extern bool smart_bone;
	extern bool prediction;
	extern bool b_recoil;
	extern float aim_smooth;
	extern int priority;
	extern const char* i_priority[];
	extern int lockon;
	extern const char* lockonmethodss[];
	extern float f_min_closest;
	extern bool b_fov;
	extern float f_fov_size;
	extern bool b_skip_knocked;
	extern int bone_index;
	extern const char* aim_lock_point[];

	//MISC
	extern bool warning_system;
	extern float warning_dist;
	extern bool fov_slide;
	extern float fov_scale;
	extern int mpwzloudout;
	extern const char* mpwzloudouts[];
	extern int classnumber;
	extern const char* classnumbers[];

	extern bool enableSnowfall;
	extern bool enableCircles;
	extern bool gameBorder;
	extern bool rgbGB;

	extern bool enabled;

	extern bool b_in_game;
	extern 	bool local_is_alive;
	extern bool is_aiming;

	extern 	int max_player_count;
	extern int connecte_players;
	extern int local_team_id;
	extern int call;
	extern 	int player_index;

	extern float bullet_speed;

	extern uintptr_t local_ptr;
	extern Vector3 local_pos;
	extern uintptr_t refdef_ptr;
	extern sdk::refdef_t* refdefadd;
	extern Vector3 view_angles;
}


namespace color
{
	extern ImColor Color;
	extern ImColor V_2dBoxes_Enemy;
	extern ImColor V_2dBoxes_F;
	extern ImColor NV_2dBoxes_Enemy;
	extern ImColor NV_2dBoxes_F;

	extern ImColor V_2ddBoxes_Enemy;
	extern ImColor V_2ddBoxes_F;
	extern ImColor NV_2ddBoxes_Enemy;
	extern ImColor NV_2ddBoxes_F;

	extern ImColor V_2dBoxes_BG_Enemy;
	extern ImColor V_2dBoxes_BG_F;
	extern ImColor NV_2dBoxes_BG_Enemy;
	extern ImColor NV_2dBoxes_BG_F;

	extern ImColor V_Bones_Enemy;
	extern ImColor V_Bones_F;
	extern ImColor NV_Bones_Enemy;
	extern ImColor NV_Bones_F;

	extern ImColor V_Bones_DEnemy;
	extern ImColor V_Bones_DF;
	extern ImColor NV_Bones_DEnemy;
	extern ImColor NV_Bones_DF;

	extern ImColor V_Bones_SREnemy;
	extern ImColor V_Bones_SRF;
	extern ImColor NV_Bones_SREnemy;
	extern ImColor NV_Bones_SRF;

	extern ImColor V_Bones_BREnemy;
	extern ImColor V_Bones_BRF;
	extern ImColor NV_Bones_BREnemy;
	extern ImColor NV_Bones_BRF;

	extern ImColor V_Line_Enemy;
	extern ImColor V_Line_F;
	extern ImColor NV_Line_Enemy;
	extern ImColor NV_Line_F;

	extern ImColor V_Friendly;
	extern ImColor NV_Friendly;

	extern ImColor V_Boxes;
	extern ImColor NV_Boxes;

	extern ImColor V_Bones;
	extern ImColor NV_Bones;

	extern ImColor V_Line;
	extern ImColor NV_Line;

	extern ImColor crosshair;
	extern ImColor showfov;

	extern ImColor radar_boarder;
	extern ImColor radar_bg;
}

namespace screenshot
{
	extern bool visuals ;
	extern bool* pDrawEnabled;
	extern	uint32_t screenshot_counter;
	extern	uint32_t  bit_blt_log;
	extern const char* bit_blt_fail;
	extern uintptr_t  bit_blt_anotherlog;
	extern	uint32_t	GdiStretchBlt_log;
	extern const char* GdiStretchBlt_fail;
	extern uintptr_t  GdiStretchBlt_anotherlog;
	extern	uintptr_t	texture_copy_log;
	extern uintptr_t virtualqueryaddr;
}

namespace customization
{
	extern ImColor text;
	extern ImColor text_disabled;
	extern ImColor windowbg;
	extern ImColor popup;
	extern ImColor Gameborder;
	extern ImColor border;
	extern ImColor framebg;
	extern ImColor framebgh;
	extern ImColor framebga;
	extern ImColor checkmark;
	extern ImColor button;
	extern ImColor buttonh;
	extern ImColor buttona;
	extern ImColor slider;
	extern ImColor header;
	extern ImColor headerh;
	extern ImColor headera;

	extern float wbs;
	extern float wr;
}