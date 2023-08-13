#include "vec.h"

namespace aimbot
{
	extern Vector3 header_to_bladder[], right_foot_to_bladder[], left_foot_to_bladder[], right_hand[], left_hand[];

	extern float init_center_dist;
	extern float min_center_dist;
	extern float init_distance;
	extern float min_distance;
	extern Vector2 lock_position;

	void is_aiming();
	int local_index();
	bool isControllerConnected();
	void reset_lock();
	unsigned long get_bone_opt();
	void start_tick();
	void update_vel_map(int index, Vector3 vPos);
	void clear_map();
	Vector3 prediction_solver(Vector3 local_pos, Vector3 position, int index, float bullet_speed);
	extern bool is_locked;
	extern bool lock_timer;
	void CG_VehicleCam_SetClientViewAngles(Vector3 newangle);
	void lock_on_target();
	bool check_valid_bones(unsigned long i);
}