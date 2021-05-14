//
// Created by Parfait Kentaurus on 5/13/21.
//

#include <fstream>

/**
 * @description	The remove_comment() function removes comment
 * 				(which starts with '#' or ';') from the string
 * @param		str
 */
void	remove_comment(std::string& str)
{
	size_t comment = str.find_first_of("#;");
	if (comment != std::string::npos)
		str.erase(comment);
}

/**
 * @description	The string_trim() function trims symbols from begin and end of the string
 * @param		str
 * @param		trim_symbols: symbols for trimming
 */
void string_trim(std::string& str, const std::string& trim_symbols)
{
	size_t begin = str.find_first_not_of(trim_symbols);
	size_t end = str.find_last_not_of(trim_symbols) + 1;
	str = str.substr(begin, end - begin);
}

/**
 * @description	The get_config_value() function gets setting value from config file
 * @param		path: path to config file
 * @param		setting: setting name
 * @return		value: setting value
 */
std::string get_config_value(const std::string& path, const std::string& setting)
{
	std::ifstream	istream(path);
	std::string		value;
	while (getline(istream, value))
	{
		remove_comment(value);
		if (value.find(setting) != std::string::npos)
		{
			size_t i = value.find('=');
			value = value.substr(i);
			string_trim(value, " \t=");
			break;
		}
	}
	return value;
}

/**
 * @description	The check_config() function checks configuration file for errors
 * @param		path: path to config file
 */
void	check_config(const std::string& path)
{
	std::ifstream	istream(path);
	std::string		line;

	if (istream.fail())
		throw std::runtime_error("Config error: can't find configuration file.");

	while (getline(istream, line))
	{
		remove_comment(line);
		if (!line.empty() && !(line[0] == ';' || line[0] == '#'))
		{
			string_trim(line, " \t");
			size_t equal_index = line.find_first_of('='); // Check for at least one '='
			if (equal_index == std::string::npos)
				throw std::runtime_error("Config error: setting must contain at least one '=");

			size_t first_space = line.find_first_of(' '); // Check for only one setting word
			if (first_space < equal_index && first_space + 1 != equal_index)
				throw std::runtime_error("Config error: setting name can't contain words separated by spaces");

			equal_index = line.find_first_of('=', equal_index + 1); // Check for multiple '=' symbols
			if (equal_index != std::string::npos)
				throw std::runtime_error("Config error: setting can contain only one '=' symbol.");

			if (line[line.find_last_not_of(" \t")] == '=') // Check for value emptiness
				throw std::runtime_error("Config error: setting value is empty");
		}
	}
}