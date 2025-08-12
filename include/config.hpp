#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct config
{
	config(const std::string& config_path);

	nlohmann::json modules;
	std::string separator;

	// add one space worth of padding to the beginning and
	// the end of the bar
	bool padding{true};
};

__attribute__((warn_unused_result))
bool validate_config_json(const nlohmann::json& j);
