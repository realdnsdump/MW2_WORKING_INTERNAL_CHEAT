#pragma once
#include "stdafx.h"
#include "vec.h"
#include <unordered_map>
#include "xor.hpp"
#include <Windows.h>
#define ror

#define _PTR_MAX_VALUE ((PVOID)0x000F000000000000)
#define BYTEn(x, n)   (*((BYTE*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)

//auto padding
#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define AutoPadding(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

#define is_valid_ptr(p) ((uint64_t)(p) <= 0x7FFFFFFEFFFF && (uint64_t)(p) >= 0x1000) 
#define is_bad_ptr(p)	(!is_valid_ptr(p))


const uintptr_t BASEIMAGE = reinterpret_cast<uint64_t>(GetModuleHandleA(NULL));

namespace bnet {
	constexpr auto game_mode = 0xF53C1A0;

	constexpr auto refdef_ptr = 0x122B28D0;
	constexpr auto name_array = 0x122CFBE8;
	constexpr auto name_array_padding = 0x5E70;
	constexpr auto name_array_size = 0xD8;

	constexpr auto camera_ptr = 0x12A11210;
	constexpr auto camera_pos = 0x1F8;

	constexpr auto local_index = 0x155C20;
	constexpr auto local_index_pos = 0x2F0;
	constexpr auto recoil = 0x155A04;

	constexpr auto distribute = 0xAFBD758;
	constexpr auto visible = 0x73E3C; 
	constexpr auto vvisible = 0x2AAB320;

	constexpr auto player_size = 0x62A0;
	constexpr auto player_valid = 0x5F41;
	constexpr auto player_pos = 0x6268;
	constexpr auto player_stance = 0xB40;
	constexpr auto player_team_id = 0x5B42;
	constexpr auto player_dead_1 = 0x5F00;
	constexpr auto player_dead_2 = 0x5ACC;

	constexpr auto bone_base_pos = 0xC6418; 
	constexpr auto bone_size = 0x188;
	constexpr auto bone_offset = 0xD8;

	constexpr auto Dvar_FindMalleableVar = 0x43F5760; 
	constexpr auto DvarSetFloat = 0x43FCBE0;

	constexpr auto playerstate = 0x77A3D20; 
	constexpr auto vehicle_viewangle = 0x74D1B70; 

	uintptr_t get_client_info();
	uint64_t get_client_info_base();
	uint64_t get_bone_ptr();
	uint64_t get_bone_index(uint32_t bone_index);
}

namespace steam {
	constexpr auto game_mode = 0xF2F0EE0; 

	constexpr auto refdef_ptr = 0x1208D2C8;
	constexpr auto name_array = 0x120AC0A8;
	constexpr auto name_array_padding = 0x5E70;
	constexpr auto name_array_size = 0xD8;

	constexpr auto camera_ptr = 0x127EC590;
	constexpr auto camera_pos = 0x1F8;

	constexpr auto local_index = 0x46E80;
	constexpr auto local_index_pos = 0x2F0; 
	constexpr auto recoil = 0x1551C8;

	constexpr auto distribute = 0xAD0E7B0;
	constexpr auto visible = 0x329AC; 
	constexpr auto vvisible = 0x2B252C0;

	constexpr auto player_size = 0x62D0;
	constexpr auto player_valid = 0x5621;
	constexpr auto player_pos = 0x5970; 
	constexpr auto player_stance = 0x4D50;
	constexpr auto player_team_id = 0x4B4D;
	constexpr auto player_dead_1 = 0x5EBA;
	constexpr auto player_dead_2 = 0xDC;

	constexpr auto bone_base_pos = 0x42120;
	constexpr auto bone_size = 0x188;
	constexpr auto bone_offset = 0xD8;

	constexpr auto playerstate = 0x79D4760;
	constexpr auto vehicle_viewangle = 0x7702640;

	uintptr_t Get_cg_t();
	uint64_t get_client_info_base();
	uint64_t get_bone_ptr();
	uint64_t get_bone_index(uint32_t bone_index);
}

namespace g_data
{
	extern uintptr_t base;
	extern HWND hWind;
	void init();
}

namespace sdk
{
	enum BONE_INDEX : unsigned long
	{
		BONE_POS_HELMET = 8,

		BONE_POS_HEAD = 7,
		BONE_POS_NECK = 6,
		BONE_POS_CHEST = 5,
		BONE_POS_MID = 4,
		BONE_POS_TUMMY = 3,
		BONE_POS_PELVIS = 2,

		BONE_POS_RIGHT_FOOT_1 = 21,
		BONE_POS_RIGHT_FOOT_2 = 22,
		BONE_POS_RIGHT_FOOT_3 = 23,
		BONE_POS_RIGHT_FOOT_4 = 24,

		BONE_POS_LEFT_FOOT_1 = 17,
		BONE_POS_LEFT_FOOT_2 = 18,
		BONE_POS_LEFT_FOOT_3 = 19,
		BONE_POS_LEFT_FOOT_4 = 20,

		BONE_POS_LEFT_HAND_1 = 13,
		BONE_POS_LEFT_HAND_2 = 14,
		BONE_POS_LEFT_HAND_3 = 15,
		BONE_POS_LEFT_HAND_4 = 16,

		BONE_POS_RIGHT_HAND_1 = 9,
		BONE_POS_RIGHT_HAND_2 = 10,
		BONE_POS_RIGHT_HAND_3 = 11,
		BONE_POS_RIGHT_HAND_4 = 12,

		BONE_RAND = 7 & 6 & 5
	};

	class clientinfo_t {
	public:
		union {
			AutoPadding(char, name[36], 0x4);
			AutoPadding(uint32_t, game_extrainfo, 0x70);
			AutoPadding(int32_t, health, 0x84);
			AutoPadding(uint8_t, squadIndex, 0x90);
			AutoPadding(Vector3, vecOrigin, 0x40);
			AutoPadding(Vector3, deltaAngle, 0x11C);
			AutoPadding(unsigned int, clientnum, 0x2A0);
			AutoPadding(Vector3, viewAngle, 0x2A4);
			AutoPadding(bool, radarEnabled, 0x42D);
			AutoPadding(bool, radarBlocked, 0x42E);
		};
	};

	enum STANCE : int
	{
		STAND = 0,
		CROUNCH = 1,
		PRONE = 2,
		KNOCKED = 3
	};

	struct ref_def_view {
		Vector2 tan_half_fov;
		char pad[0xC];
		Vector3 axis[3];
	};

	class refdef_t {
	public:
		int x;
		int y;
		int Width;
		int Height;
		ref_def_view view;
	};

	class player_t
	{
	public:
		player_t(uintptr_t address) {
			this->address = address;
		}
		uintptr_t address{};

		uintptr_t get_index();

		int get_stance();

		bool is_valid();

		bool is_dead();

		int team_id();

		Vector3 get_pos();
	};

	bool in_game();
	int get_max_player_count();
	refdef_t* get_refdef();
	player_t get_player(int i);
	player_t get_local_player();
	std::string get_player_name(int i);
	bool get_bone_by_player_index(int i, int bone_index, Vector3* Out_bone_pos);
	int get_player_health(int i);
}
