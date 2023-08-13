namespace esp
{
	namespace g_draw
	{
		void draw_line(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness);
		void draw_sketch_edge_text(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, uint32_t color, bool center, uint32_t EdgeColor = 0xFF000000);
		void draw_crosshair();
		void draw_fov(const float aimbot_fov);
	}

	bool w2s(Vector3 world_location, Vector2* ScreenPos);
	bool wts(Vector3& W_POS, Vector2* S_POS);
	void d_bones(Vector3 from, Vector3 to, Vector3 dpos, ImColor color);
}