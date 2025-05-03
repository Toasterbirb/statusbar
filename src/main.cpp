#include <clipp.h>
#include <filesystem>
#include <format>
#include <iostream>

#include "config.hpp"
#include "status.hpp"

int main(int argc, char** argv)
{
	bool show_help{false};
	bool verbose{false};
	std::string config_path;

	auto cli = (
		clipp::option("--help", "-h").set(show_help)
		% "display this help page you are reading",

		(clipp::option("--config", "-c") & clipp::value("path").set(config_path))
		% "set a custom configuration file path",

		clipp::option("--verbose").set(verbose)
		% "print the status bar updates to stdout"
	);

	if (!clipp::parse(argc, argv, cli) || show_help)
	{
		std::cout << clipp::make_man_page(cli, "statusbar");

		if (!show_help)
			return 1;

		return 0;
	}

	// if no custom config path was set, use the default path
	if (config_path.empty())
		config_path = std::format("{}/.config/statusbar.json", getenv("HOME"));

	// check if the config file exists
	if (!std::filesystem::exists(config_path))
	{
		std::cerr << "config file " << config_path << " does not exist\n";
		return 1;
	}

	// parse the config file
	const config cfg(config_path);

	// start the daemon loop
	status_loop(cfg, verbose);
}
