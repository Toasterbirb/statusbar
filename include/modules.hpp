#pragma once

#include <nlohmann/json.hpp>
#include <string>

namespace module
{
	std::string text(const nlohmann::json& module_cfg);
	std::string shell(const nlohmann::json& module_cfg);
}

const static std::map<std::string, std::function<std::string(const nlohmann::json& module_cfg)>> modules = {
	{ "text", module::text },
	{ "shell", module::shell }
};
