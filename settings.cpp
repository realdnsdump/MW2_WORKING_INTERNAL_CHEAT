#include "globals.h"

namespace Settings
{
	std::string str_config_name = "";

	BOOL WritePrivateProfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, int nInteger, LPCSTR lpFileName) {
		char lpString[1024];
		sprintf(lpString, "%d", nInteger);
		return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
	}
	BOOL WritePrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, float nInteger, LPCSTR lpFileName) {
		char lpString[1024];
		sprintf(lpString, "%f", nInteger);
		return WritePrivateProfileStringA(lpAppName, lpKeyName, lpString, lpFileName);
	}
	float GetPrivateProfileFloat(LPCSTR lpAppName, LPCSTR lpKeyName, FLOAT flDefault, LPCSTR lpFileName)
	{
		char szData[32];

		GetPrivateProfileStringA(lpAppName, lpKeyName, std::to_string(flDefault).c_str(), szData, 32, lpFileName);

		return (float)atof(szData);
	}

	void Save_Settings(std::string fileName)
	{
		char file_path[MAX_PATH];
		sprintf_s(file_path, XOR("C:\\Configs\\%s%s"), fileName.c_str(), XOR(".ini"));
		WritePrivateProfileInt("aimbot", "enable_aimbot", globals::b_lock, file_path);
		WritePrivateProfileInt("aimbot", "visible_check", globals::b_visible, file_path);
		WritePrivateProfileInt("aimbot", "disable_down", globals::disable_bd, file_path);
		WritePrivateProfileInt("aimbot", "V_Bones_DEnemy", color::V_Bones_DEnemy, file_path);
		WritePrivateProfileInt("aimbot", "V_Bones_DF", color::V_Bones_DF, file_path);
		WritePrivateProfileInt("aimbot", "NV_Bones_DEnemy", color::NV_Bones_DEnemy, file_path);
		WritePrivateProfileInt("aimbot", "NV_Bones_DF", color::NV_Bones_DF, file_path);
		WritePrivateProfileInt("aimbot", "self_revive", globals::self_revive, file_path);
		WritePrivateProfileInt("aimbot", "V_Bones_SREnemy", color::V_Bones_SREnemy, file_path);
		WritePrivateProfileInt("aimbot", "V_Bones_SRF", color::V_Bones_SRF, file_path);
		WritePrivateProfileInt("aimbot", "NV_Bones_SREnemy", color::NV_Bones_SREnemy, file_path);
		WritePrivateProfileInt("aimbot", "NV_Bones_SRF", color::NV_Bones_SRF, file_path);
		WritePrivateProfileInt("aimbot", "b_revive", globals::b_revive, file_path);
		WritePrivateProfileInt("aimbot", "V_Bones_BREnemy", color::V_Bones_BREnemy, file_path);
		WritePrivateProfileInt("aimbot", "V_Bones_BRF", color::V_Bones_BRF, file_path);
		WritePrivateProfileInt("aimbot", "NV_Bones_BREnemy", color::NV_Bones_BREnemy, file_path);
		WritePrivateProfileInt("aimbot", "NV_Bones_BRF", color::NV_Bones_BRF, file_path);
		WritePrivateProfileInt("aimbot", "bone_scan", globals::bone_scan, file_path);
		WritePrivateProfileInt("aimbot", "smart_bone", globals::smart_bone, file_path);
		WritePrivateProfileInt("aimbot", "prediction", globals::prediction, file_path);
		WritePrivateProfileFloat("aimbot", "smoothness", globals::aim_smooth, file_path);
		WritePrivateProfileInt("aimbot", "target_priority", globals::priority, file_path);
		WritePrivateProfileInt("aimbot", "lockon", globals::lockon, file_path);
		WritePrivateProfileFloat("aimbot", "fov_circle_dist", globals::f_min_closest, file_path);
		WritePrivateProfileInt("aimbot", "fov_circle", globals::b_fov, file_path);
		WritePrivateProfileFloat("aimbot", "fov_circle_size", globals::f_fov_size, file_path);
		WritePrivateProfileInt("aimbot", "skip_knocked", globals::b_skip_knocked, file_path);
		WritePrivateProfileInt("aimbot", "bone_index", globals::bone_index, file_path);
		WritePrivateProfileInt("esp", "visible_only", globals::vis_only, file_path);
		WritePrivateProfileInt("esp", "crosshair", globals::b_crosshair, file_path);
		WritePrivateProfileInt("esp", "crosshair_type", globals::b_crosshair_type, file_path);
		WritePrivateProfileFloat("esp", "crosshair_thickness", globals::b_crosshair_thickness, file_path);
		WritePrivateProfileFloat("esp", "crosshair_size", globals::b_crosshair_size, file_path);
		WritePrivateProfileInt("esp", "crosshair_color", color::crosshair, file_path);
		WritePrivateProfileInt("esp", "diamond", globals::esp_box, file_path);
		WritePrivateProfileInt("esp", "diamond_color_v_e", color::V_2ddBoxes_Enemy, file_path);
		WritePrivateProfileInt("esp", "diamond_color_v_f", color::V_2ddBoxes_F, file_path);
		WritePrivateProfileInt("esp", "diamond_color_nv_e", color::NV_2ddBoxes_Enemy, file_path);
		WritePrivateProfileInt("esp", "diamond_color_nv_f", color::NV_2ddBoxes_F, file_path);
		WritePrivateProfileInt("esp", "2d_box", globals::b_box, file_path);
		WritePrivateProfileInt("esp", "2d_box_color_v_e", color::V_2dBoxes_Enemy, file_path);
		WritePrivateProfileInt("esp", "2d_box_color_v_f", color::V_2dBoxes_F, file_path);
		WritePrivateProfileInt("esp", "2d_box_color_nv_e", color::NV_2dBoxes_Enemy, file_path);
		WritePrivateProfileInt("esp", "2d_box_color_nv_f", color::NV_2dBoxes_F, file_path);
		WritePrivateProfileFloat("esp", "box_thickness", globals::box_thickness, file_path);
		WritePrivateProfileInt("esp", "box_fill", globals::box_bg, file_path);
		WritePrivateProfileInt("esp", "box_fill_color_v_e", color::V_2dBoxes_BG_Enemy, file_path);
		WritePrivateProfileInt("esp", "box_fill_color_v_f", color::V_2dBoxes_BG_F, file_path);
		WritePrivateProfileInt("esp", "box_fill_color_nv_e", color::NV_2dBoxes_BG_Enemy, file_path);
		WritePrivateProfileInt("esp", "box_fill_color_nv_f", color::NV_2dBoxes_BG_F, file_path);
		WritePrivateProfileInt("esp", "snaplines", globals::b_line, file_path);
		WritePrivateProfileInt("esp", "snaplines_type", globals::linetype, file_path);
		WritePrivateProfileFloat("esp", "snaplines_thickness", globals::lines_thickness, file_path);
		WritePrivateProfileInt("esp", "snaplines_color_v_e", color::V_Line_Enemy, file_path);
		WritePrivateProfileInt("esp", "snaplines_color_v_f", color::V_Line_F, file_path);
		WritePrivateProfileInt("esp", "snaplines_color_nv_e", color::NV_Line_Enemy, file_path);
		WritePrivateProfileInt("esp", "snaplines_color_nv_f", color::NV_Line_F, file_path);
		WritePrivateProfileInt("esp", "skeleton", globals::b_skeleton, file_path);
		WritePrivateProfileInt("esp", "skeleton_head", globals::head_circle, file_path);
		WritePrivateProfileFloat("esp", "skeleton_thickness", globals::bones_thickness, file_path);
		WritePrivateProfileInt("esp", "skeleton_color_v_e", color::V_Bones_Enemy, file_path);
		WritePrivateProfileInt("esp", "skeleton_color_v_f", color::V_Bones_F, file_path);
		WritePrivateProfileInt("esp", "skeleton_color_nv_e", color::NV_Bones_Enemy, file_path);
		WritePrivateProfileInt("esp", "skeleton_color_nv_f", color::NV_Bones_F, file_path);
		WritePrivateProfileInt("esp", "names", globals::b_namee, file_path);
		WritePrivateProfileInt("esp", "distance", globals::distance, file_path);
		WritePrivateProfileInt("esp", "healthbar", globals::draw_health, file_path);
		WritePrivateProfileInt("esp", "team_id", globals::team_id, file_path);
		WritePrivateProfileInt("esp", "show_team", globals::b_friendly, file_path);
		WritePrivateProfileInt("esp", "max_distance", globals::max_distance, file_path);
		WritePrivateProfileFloat("esp", "text_size", globals::esp_text_size, file_path);
		WritePrivateProfileInt("misc", "warning_system", globals::warning_system, file_path);
		WritePrivateProfileInt("misc", "warning_distance", globals::warning_dist, file_path);
		WritePrivateProfileInt("misc", "fov_slider", globals::fov_slide, file_path);
		WritePrivateProfileFloat("misc", "fov_slideer_scale", globals::fov_scale, file_path);
		WritePrivateProfileInt("misc", "recoil", globals::b_recoil, file_path);
		WritePrivateProfileInt("misc", "enableSnowfall", globals::enableSnowfall, file_path);
		WritePrivateProfileInt("misc", "enableCircles", globals::enableCircles, file_path);
		WritePrivateProfileInt("misc", "gameBorder", globals::gameBorder, file_path);
		WritePrivateProfileInt("misc", "rgbGB", globals::rgbGB, file_path);
		WritePrivateProfileInt("customization", "text", customization::text, file_path);
		WritePrivateProfileInt("customization", "text_disabled", customization::text_disabled, file_path);
		WritePrivateProfileInt("customization", "windowbg", customization::windowbg, file_path);
		WritePrivateProfileInt("customization", "popup", customization::popup, file_path);
		WritePrivateProfileInt("customization", "border", customization::border, file_path);
		WritePrivateProfileInt("customization", "Gameborder", customization::Gameborder, file_path);
		WritePrivateProfileInt("customization", "framebg", customization::framebg, file_path);
		WritePrivateProfileInt("customization", "framebgh", customization::framebgh, file_path);
		WritePrivateProfileInt("customization", "framebga", customization::framebga, file_path);
		WritePrivateProfileInt("customization", "checkmark", customization::checkmark, file_path);
		WritePrivateProfileInt("customization", "button", customization::button, file_path);
		WritePrivateProfileInt("customization", "buttonh", customization::buttonh, file_path);
		WritePrivateProfileInt("customization", "buttona", customization::buttona, file_path);
		WritePrivateProfileInt("customization", "slider", customization::slider, file_path);
		WritePrivateProfileInt("customization", "header", customization::header, file_path);
		WritePrivateProfileInt("customization", "headerh", customization::headerh, file_path);
		WritePrivateProfileInt("customization", "headera", customization::headera, file_path);
		WritePrivateProfileFloat("customization", "wbs", customization::wbs, file_path);
		WritePrivateProfileFloat("customization", "wr", customization::wr, file_path);
	}

	void Load_Settings(std::string fileName)
	{
		char file_path[MAX_PATH];
		sprintf_s(file_path, XOR("C:\\Configs\\%s%s"), fileName.c_str(), XOR(".ini"));
		globals::b_lock = GetPrivateProfileIntA("aimbot", "enable_aimbot", globals::b_lock, file_path);
		globals::b_visible = GetPrivateProfileIntA("aimbot", "visible_check", globals::b_visible, file_path);
		globals::disable_bd = GetPrivateProfileIntA("aimbot", "disable_down", globals::disable_bd, file_path);
		color::V_Bones_DEnemy = GetPrivateProfileIntA("aimbot", "V_Bones_DEnemy", color::V_Bones_DEnemy, file_path);
		color::V_Bones_DF = GetPrivateProfileIntA("aimbot", "V_Bones_DF", color::V_Bones_DF, file_path);
		color::NV_Bones_DEnemy = GetPrivateProfileIntA("aimbot", "NV_Bones_DEnemy", color::NV_Bones_DEnemy, file_path);
		color::NV_Bones_DF = GetPrivateProfileIntA("aimbot", "NV_Bones_DF", color::NV_Bones_DF, file_path);
		globals::self_revive = GetPrivateProfileIntA("aimbot", "self_revive", globals::self_revive, file_path);
		color::V_Bones_SREnemy = GetPrivateProfileIntA("aimbot", "V_Bones_SREnemy", color::V_Bones_SREnemy, file_path);
		color::V_Bones_SRF = GetPrivateProfileIntA("aimbot", "V_Bones_SRF", color::V_Bones_SRF, file_path);
		color::NV_Bones_SREnemy = GetPrivateProfileIntA("aimbot", "NV_Bones_SREnemy", color::NV_Bones_SREnemy, file_path);
		color::NV_Bones_SRF = GetPrivateProfileIntA("aimbot", "NV_Bones_SRF", color::NV_Bones_SRF, file_path);
		globals::b_revive = GetPrivateProfileIntA("aimbot", "b_revive", globals::b_revive, file_path);
		color::V_Bones_BREnemy = GetPrivateProfileIntA("aimbot", "V_Bones_BREnemy", color::V_Bones_BREnemy, file_path);
		color::V_Bones_BRF = GetPrivateProfileIntA("aimbot", "V_Bones_BRF", color::V_Bones_BRF, file_path);
		color::NV_Bones_BREnemy = GetPrivateProfileIntA("aimbot", "NV_Bones_BREnemy", color::NV_Bones_BREnemy, file_path);
		color::NV_Bones_BRF = GetPrivateProfileIntA("aimbot", "NV_Bones_BRF", color::NV_Bones_BRF, file_path);
		globals::bone_scan = GetPrivateProfileIntA("aimbot", "bone_scan", globals::bone_scan, file_path);
		globals::smart_bone = GetPrivateProfileIntA("aimbot", "smart_bone", globals::smart_bone, file_path);
		globals::prediction = GetPrivateProfileIntA("aimbot", "prediction", globals::prediction, file_path);
		globals::aim_smooth = GetPrivateProfileFloat("aimbot", "smoothness", globals::aim_smooth, file_path);
		globals::priority = GetPrivateProfileIntA("aimbot", "target_priority", globals::priority, file_path);
		globals::lockon = GetPrivateProfileIntA("aimbot", "lockon", globals::lockon, file_path);
		globals::f_min_closest = GetPrivateProfileFloat("aimbot", "fov_circle_dist", globals::f_min_closest, file_path);
		globals::b_fov = GetPrivateProfileIntA("aimbot", "fov_circle", globals::b_fov, file_path);
		globals::f_fov_size = GetPrivateProfileFloat("aimbot", "fov_circle_size", globals::f_fov_size, file_path);
		globals::b_skip_knocked = GetPrivateProfileIntA("aimbot", "skip_knocked", globals::b_skip_knocked, file_path);
		globals::bone_index = GetPrivateProfileIntA("aimbot", "bone_index", globals::bone_index, file_path);
		globals::vis_only = GetPrivateProfileIntA("esp", "visible_only", globals::vis_only, file_path);
		globals::b_crosshair = GetPrivateProfileIntA("esp", "crosshair", globals::b_crosshair, file_path);
		globals::b_crosshair_type = GetPrivateProfileIntA("esp", "crosshair_type", globals::b_crosshair_type, file_path);
		globals::b_crosshair_thickness = GetPrivateProfileFloat("esp", "crosshair_thickness", globals::b_crosshair_thickness, file_path);
		globals::b_crosshair_size = GetPrivateProfileFloat("esp", "crosshair_size", globals::b_crosshair_size, file_path);
		color::crosshair = GetPrivateProfileIntA("esp", "crosshair_color", color::crosshair, file_path);
		globals::esp_box = GetPrivateProfileIntA("esp", "diamond", globals::esp_box, file_path);
		color::V_2ddBoxes_Enemy = GetPrivateProfileIntA("esp", "diamond_color_v_e", color::V_2ddBoxes_Enemy, file_path);
		color::V_2ddBoxes_F = GetPrivateProfileIntA("esp", "diamond_color_v_f", color::V_2ddBoxes_F, file_path);
		color::NV_2ddBoxes_Enemy = GetPrivateProfileIntA("esp", "diamond_color_nv_e", color::NV_2ddBoxes_Enemy, file_path);
		color::NV_2ddBoxes_F = GetPrivateProfileIntA("esp", "diamond_color_nv_f", color::NV_2ddBoxes_F, file_path);
		globals::b_box = GetPrivateProfileIntA("esp", "2d_box", globals::b_box, file_path);
		color::V_2dBoxes_Enemy = GetPrivateProfileIntA("esp", "2d_box_color_v_e", color::V_2dBoxes_Enemy, file_path);
		color::V_2dBoxes_F = GetPrivateProfileIntA("esp", "2d_box_color_v_f", color::V_2dBoxes_F, file_path);
		color::NV_2dBoxes_Enemy = GetPrivateProfileIntA("esp", "2d_box_color_nv_e", color::NV_2dBoxes_Enemy, file_path);
		color::NV_2dBoxes_F = GetPrivateProfileIntA("esp", "2d_box_color_nv_f", color::NV_2dBoxes_F, file_path);
		globals::box_thickness = GetPrivateProfileFloat("esp", "box_thickness", globals::box_thickness, file_path);
		globals::box_bg = GetPrivateProfileIntA("esp", "box_fill", globals::box_bg, file_path);
		color::V_2dBoxes_BG_Enemy = GetPrivateProfileIntA("esp", "box_fill_color_v_e", color::V_2dBoxes_BG_Enemy, file_path);
		color::V_2dBoxes_BG_F = GetPrivateProfileIntA("esp", "box_fill_color_v_f", color::V_2dBoxes_BG_F, file_path);
		color::NV_2dBoxes_BG_Enemy = GetPrivateProfileIntA("esp", "box_fill_color_nv_e", color::NV_2dBoxes_BG_Enemy, file_path);
		color::NV_2dBoxes_BG_F = GetPrivateProfileIntA("esp", "box_fill_color_nv_f", color::NV_2dBoxes_BG_F, file_path);
		globals::b_line = GetPrivateProfileIntA("esp", "snaplines", globals::b_line, file_path);
		globals::linetype = GetPrivateProfileIntA("esp", "snaplines_type", globals::linetype, file_path);
		globals::lines_thickness = GetPrivateProfileFloat("esp", "snaplines_thickness", globals::lines_thickness, file_path);
		color::V_Line_Enemy = GetPrivateProfileIntA("esp", "snaplines_color_v_e", color::V_Line_Enemy, file_path);
		color::V_Line_F = GetPrivateProfileIntA("esp", "snaplines_color_v_f", color::V_Line_F, file_path);
		color::NV_Line_Enemy = GetPrivateProfileIntA("esp", "snaplines_color_nv_e", color::NV_Line_Enemy, file_path);
		color::NV_Line_F = GetPrivateProfileIntA("esp", "snaplines_color_nv_f", color::NV_Line_F, file_path);
		globals::b_skeleton = GetPrivateProfileIntA("esp", "skeleton", globals::b_skeleton, file_path);
		globals::head_circle = GetPrivateProfileIntA("esp", "skeleton_head", globals::head_circle, file_path);
		globals::bones_thickness = GetPrivateProfileFloat("esp", "skeleton_thickness", globals::bones_thickness, file_path);
		color::V_Bones_Enemy = GetPrivateProfileIntA("esp", "skeleton_color_v_e", color::V_Bones_Enemy, file_path);
		color::V_Bones_F = GetPrivateProfileIntA("esp", "skeleton_color_v_f", color::V_Bones_F, file_path);
		color::NV_Bones_Enemy = GetPrivateProfileIntA("esp", "skeleton_color_nv_e", color::NV_Bones_Enemy, file_path);
		color::NV_Bones_F = GetPrivateProfileIntA("esp", "skeleton_color_nv_f", color::NV_Bones_F, file_path);
		globals::b_namee = GetPrivateProfileIntA("esp", "names", globals::b_namee, file_path);
		globals::distance = GetPrivateProfileIntA("esp", "distance", globals::distance, file_path);
		globals::draw_health = GetPrivateProfileIntA("esp", "healthbar", globals::draw_health, file_path);
		globals::team_id = GetPrivateProfileIntA("esp", "team_id", globals::team_id, file_path);
		globals::b_friendly = GetPrivateProfileIntA("esp", "show_team", globals::b_friendly, file_path);
		globals::max_distance = GetPrivateProfileIntA("esp", "max_distance", globals::max_distance, file_path);
		globals::esp_text_size = GetPrivateProfileFloat("esp", "text_size", globals::esp_text_size, file_path);
		globals::warning_system = GetPrivateProfileIntA("misc", "warning_system", globals::warning_system, file_path);
		globals::warning_dist = GetPrivateProfileIntA("misc", "warning_distance", globals::warning_dist, file_path);
		globals::fov_slide = GetPrivateProfileIntA("misc", "fov_slider", globals::fov_slide, file_path);
		globals::fov_scale = GetPrivateProfileFloat("misc", "fov_slideer_scale", globals::fov_scale, file_path);
		globals::b_recoil = GetPrivateProfileIntA("misc", "recoil", globals::b_recoil, file_path);
		globals::enableSnowfall = GetPrivateProfileIntA("misc", "enableSnowfall", globals::enableSnowfall, file_path);
		globals::enableCircles = GetPrivateProfileIntA("misc", "enableCircles", globals::enableCircles, file_path);
		globals::gameBorder = GetPrivateProfileIntA("misc", "gameBorder", globals::gameBorder, file_path);
		globals::rgbGB = GetPrivateProfileIntA("misc", "rgbGB", globals::rgbGB, file_path);
		customization::text = GetPrivateProfileIntA("customization", "text", customization::text, file_path);
		customization::text_disabled = GetPrivateProfileIntA("customization", "text_disabled", customization::text_disabled, file_path);
		customization::windowbg = GetPrivateProfileIntA("customization", "windowbg", customization::windowbg, file_path);
		customization::popup = GetPrivateProfileIntA("customization", "popup", customization::popup, file_path);
		customization::border = GetPrivateProfileIntA("customization", "border", customization::border, file_path);
		customization::Gameborder = GetPrivateProfileIntA("customization", "Gameborder", customization::Gameborder, file_path);
		customization::framebg = GetPrivateProfileIntA("customization", "framebg", customization::framebg, file_path);
		customization::framebgh = GetPrivateProfileIntA("customization", "framebgh", customization::framebgh, file_path);
		customization::framebga = GetPrivateProfileIntA("customization", "framebga", customization::framebga, file_path);
		customization::checkmark = GetPrivateProfileIntA("customization", "checkmark", customization::checkmark, file_path);
		customization::button = GetPrivateProfileIntA("customization", "button", customization::button, file_path);
		customization::buttonh = GetPrivateProfileIntA("customization", "buttonh", customization::buttonh, file_path);
		customization::buttona = GetPrivateProfileIntA("customization", "buttona", customization::buttona, file_path);
		customization::slider = GetPrivateProfileIntA("customization", "slider", customization::slider, file_path);
		customization::header = GetPrivateProfileIntA("customization", "header", customization::header, file_path);
		customization::headerh = GetPrivateProfileIntA("customization", "headerh", customization::headerh, file_path);
		customization::headera = GetPrivateProfileIntA("customization", "headera", customization::headera, file_path);
		customization::wbs = GetPrivateProfileFloat("customization", "wbs", customization::wbs, file_path);
		customization::wr = GetPrivateProfileFloat("customization", "wr", customization::wr, file_path);
		str_config_name = fileName;
	};

	std::vector<std::string> GetList()
	{
		std::vector<std::string> configs;
		WIN32_FIND_DATA ffd;
		LPCSTR directory = XOR("C:\\Configs\\*");
		auto hFind = FindFirstFile(directory, &ffd);
		while (FindNextFile(hFind, &ffd))
		{
			if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				std::string file_name = ffd.cFileName;
				if (file_name.size() < 4) // .cfg
					continue;
				std::string end = file_name;
				end.erase(end.begin(), end.end() - 4);
				if (end != XOR(".ini"))
					continue;
				file_name.erase(file_name.end() - 4, file_name.end());
				configs.push_back(file_name);
			}
		}
		return configs;
	}

}