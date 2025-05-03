#include <array>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "config.hpp"

config::config(const std::string& config_path)
{
	assert(!config_path.empty());

	// this should be checked in main() during runtime
	assert(std::filesystem::exists(config_path));

	std::ifstream file(config_path);
	if (!file.is_open())
	{
		std::cerr << "can't open file " << config_path << '\n';
		exit(1);
	}

	try
	{
		const nlohmann::json cfg_json = nlohmann::json::parse(file);
		if (!validate_config_json(cfg_json))
		{
			std::cerr << "the configuration file has errors in it, not proceeding any further\n";
			exit(1);
		}

		this->modules = cfg_json.at("modules");
		this->separator = cfg_json.at("separator");
	}
	catch (const nlohmann::json::exception& ex)
	{
		std::cerr << "failed to parse the configuration file\n"
			<< ex.what() << '\n';
	}
}

bool validate_config_json(const nlohmann::json& j)
{
	enum class key_type
	{
		string, number, array
	};

	// define all required keys here
	constexpr static std::array<std::pair<std::string, key_type>, 2> required_keys = {
		std::pair<std::string, key_type>{ "modules", key_type::array },
		{ "separator", key_type::string }
	};

	const auto type_error = [](const std::string& key_name, const std::string& type_name)
	{
		std::cerr << key_name << " has a wrong type (expected " << type_name << ")\n";
	};

	for (const auto& [key_name, type] : required_keys)
	{
		if (!j.contains(key_name))
		{
			std::cout << "the configuration file is missing a required key: " << key_name << '\n';
			return false;
		}

		switch (type)
		{
			case key_type::string:
			{
				if (!j.at(key_name).is_string())
				{
					type_error(key_name, "string");
					return false;
				}
				break;
			}

			case key_type::number:
			{
				if (!j.at(key_name).is_number())
				{
					type_error(key_name, "number");
					return false;
				}
				break;
			}

			case key_type::array:
			{
				if (!j.at(key_name).is_array())
				{
					type_error(key_name, "array");
					return false;
				}
				break;
			}
		}
	}

	// validate modules
	for (const nlohmann::json& module : j.at("modules"))
	{
		// all modules should have at least a name
		if (!module.contains("name"))
		{
			std::cerr << "came across a module with no name\n";
			return false;
		}
	}

	return true;
}
