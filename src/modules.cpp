#include <cstdio>
#include <stdio.h>

#include "modules.hpp"

namespace module
{
	std::string text(const nlohmann::json& module_cfg)
	{
		return module_cfg.at("text");
	}

	std::string shell(const nlohmann::json& module_cfg)
	{
		const std::string& cmd = module_cfg.at("cmd");
		FILE* pipe = popen(cmd.c_str(), "r");
		if (!pipe)
		{
			// if the pipe fails to open, ignore the command output
			return "";
		}

		std::string text;
		constexpr size_t buf_size = 256;
		char buffer[buf_size];

		while (fgets(buffer, buf_size, pipe))
			text += buffer;

		pclose(pipe);

		// pop off the newline at the end of the command printed out one
		if (!text.empty() && text.at(text.size() - 1) == '\n')
			text.pop_back();

		return text;
	}
}
