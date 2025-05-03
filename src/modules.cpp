#include <cstdio>
#include <format>
#include <fstream>
#include <stdio.h>

#include "modules.hpp"
#include "types.hpp"

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

		// pop off the newline at the end of the command if it printed out one
		if (!text.empty() && text.at(text.size() - 1) == '\n')
			text.pop_back();

		return text;
	}

	// copied over from birb3d
	// https://github.com/toasterbirb/birb3d
	constexpr f64 round(const f64 value, const i8 decimal_points)
	{
		/* How this thing works:
		 * 1. Multiply the value with 10 ^ decimal points. This will leave the needed values before the decimal point
		 * 2. Round to integer
		 * 3. Divide the value with 10 ^ decimal points to get the desired rounded decimal value
		 * */
		return std::round(value * std::pow(10, decimal_points)) / static_cast<f64>(std::pow(10, decimal_points));
	}

	// copied over from the meminfo project
	// http://birbgitfh224rep6tmdofmr6qlo6wx43umqzt3hjubnncr55sdlfmtad.onion/toasterbirb/meminfo
	std::string memory(const nlohmann::json& module_cfg)
	{
		u64 mem_total{};
		u64 total_free{};

		{
			std::fstream meminfo("/proc/meminfo", std::ios::in);

			// don't attempt to read from the meminfo file if it can't be opened
			if (!meminfo.is_open())
				return "";

			std::string name;
			u64 value;
			std::string unit;

			// Read in the amount of available memory and swap
			while (meminfo >> name >> value >> unit)
			{
				if (name == "MemTotal:" || name == "SwapTotal:")
				{
					mem_total += value;
					continue;
				}

				if (name == "MemAvailable:" || name == "SwapFree:")
					total_free += value;
			}
		}

		// Convert the free memory from kilobytes to gigabytes
		constexpr f32 divisor = 1.0f / 1024.0f / 1024.0f;
		const f32 total_free_gb = total_free * divisor;
		const f32 total_mem_gb = mem_total * divisor;

		// Increase precision if the amount of free memory is more than 10 gigabytes
		const i32 precision = module_cfg.contains("decimals") ? (i32)module_cfg.at("decimals") : 1;

		return std::format("{}G/{}G", round(total_mem_gb - total_free_gb, precision), round(total_mem_gb, precision));
	}
}
