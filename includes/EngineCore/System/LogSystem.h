#pragma once

#include <string>
#include <vector>

namespace std
{
	string to_string(string str);
	string to_string(wstring str);
	wstring to_wstring(string str);
	wstring to_wstring(wstring str);
}

class LogSystem
{	
public:
	~LogSystem() = delete;
	LogSystem() = delete;

	LogSystem(const LogSystem&) = delete;
	LogSystem(const LogSystem&&) = delete;
	LogSystem& operator=(const LogSystem&) = delete;
	LogSystem& operator=(const LogSystem&&) = delete;
	
	static void init_log_system(std::string relPathFolder);
	static void uninit_log_system();

	static void log_info(std::string msg);
	static void log_warn(std::string msg);
	static void log_error(std::string msg);
	static void log_crit(std::string msg);

	template<typename... _Args>
	static void info(std::string text, _Args &&...args)
	{
		std::vector<std::string> strs;

		(strs.push_back(std::to_string(args)), ...);

		while (text.find('{') != -1)
		{
			int a = text.find('{');
			int b = text.find('}', a);
			std::string s = text.substr(a + 1, 1);
			int i = std::stoi(s);
			text = text.substr(0, a) + strs[i] + text.substr(b + 1);
		}
		log_info(text);
	}

	template<typename... _Args>
	static void warn(std::string text, _Args &&...args)
	{
		std::vector<std::string> strs;

		(strs.push_back(std::to_string(args)), ...);

		while (text.find('{') != -1)
		{
			int a = text.find('{');
			int b = text.find('}', a);
			std::string s = text.substr(a + 1, 1);
			int i = std::stoi(s);
			text = text.substr(0, a) + strs[i] + text.substr(b + 1);
		}
		log_warn(text);
	}

	template<typename... _Args>
	static void error(std::string text, _Args &&...args)
	{
		std::vector<std::string> strs;

		(strs.push_back(std::to_string(args)), ...);

		while (text.find('{') != -1)
		{
			int a = text.find('{');
			int b = text.find('}', a);
			std::string s = text.substr(a + 1, 1);
			int i = std::stoi(s);
			text = text.substr(0, a) + strs[i] + text.substr(b + 1);
		}
		log_error(text);
	}

	template<typename... _Args>
	static void critical(std::string text, _Args &&...args)
	{
		std::vector<std::string> strs;

		(strs.push_back(std::to_string(args)), ...);

		while (text.find('{') != -1)
		{
			int a = text.find('{');
			int b = text.find('}', a);
			std::string s = text.substr(a + 1, 1);
			int i = std::stoi(s);
			text = text.substr(0, a) + strs[i] + text.substr(b + 1);
		}
		log_crit(text);
	}
private:
	static std::string m_path;
};