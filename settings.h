namespace Settings
{
	extern std::string str_config_name;
	void Save_Settings(std::string path);
	void Load_Settings(std::string path);
	std::vector<std::string> GetList();
}