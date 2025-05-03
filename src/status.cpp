#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <X11/Xlib.h>

#include "modules.hpp"
#include "status.hpp"

using namespace std::chrono_literals;

class module_state
{
public:
	module_state(const nlohmann::json& module_json)
	:module_json(module_json)
	{
		if (module_json.at("name") == "text")
			is_static = true;

		// don't waste threads on static modules
		if (!is_static)
			run();
		else
			text = modules.at(module_json.at("name"))(module_json);
	}

	const nlohmann::json module_json;

	void poll()
	{
		// static modules need to be only updated once
		if (is_static)
			return;

		constexpr static const char* interval_key = "interval";
		constexpr static size_t default_interval_s = 1;

		const long interval_s = module_json.contains(interval_key)
			? (long)module_json.at(interval_key)
			: default_interval_s;

		const auto current_time = std::chrono::system_clock::now();
		const auto duration = std::chrono::duration(current_time - last_run);

		if (std::chrono::duration_cast<std::chrono::seconds>(duration).count() < interval_s && !is_first_run)
			return;

		if (is_ready())
		{
			this->text = future.get();
			run();

			is_first_run = false;
		}
	}

	std::string txt() const
	{
		return text;
	}

private:
	std::future<std::string> future;
	std::chrono::time_point<std::chrono::system_clock> last_run;
	std::string text;

	bool is_static{false};
	bool is_first_run{true};

	void run()
	{
		future = std::async(std::launch::async, modules.at(module_json.at("name")), module_json);
		last_run = std::chrono::system_clock::now();
	}

	bool is_ready() const
	{
		assert(future.valid());
		assert(last_run.time_since_epoch().count() != 0);
		return future.wait_for(0ms) == std::future_status::ready;
	}
};

void status_loop(const config& cfg, const bool verbose)
{
	// Setup X11 stuff
	// refer to slstatus (https://tools.suckless.org/slstatus/)
	// if you need more information on how this might work
	Display* const dpy = XOpenDisplay(NULL);
	if (!dpy)
	{
		std::cerr << "failed to open display\n";
		exit(1);
	}

	// keep track of the previous status text to avoid unnecessary uphttps://tools.suckless.org/slstatus/dates
	std::string old_status_line = " ";

	std::vector<module_state> state_arr;
	for (size_t i = 0; i < cfg.modules.size(); ++i)
		state_arr.push_back({ cfg.modules.at(i) });

	while (true)
	{
		for (size_t i = 0; i < cfg.modules.size(); ++i)
		{
			const std::string& module_name = cfg.modules.at(i).at("name");
			if (!modules.contains(module_name))
			{
				std::cerr << "non-existent module: " << module_name << '\n';
				XCloseDisplay(dpy);
				exit(1);
			}

			state_arr.at(i).poll();
		}

		// build the status line
		std::string status_line = " ";
		for (size_t i = 0; i < state_arr.size(); ++i)
		{
			const std::string& text = state_arr.at(i).txt();
			if (text.empty())
				continue;

			// print the icon if the module has one
			if (state_arr.at(i).module_json.contains("icon"))
				status_line += state_arr.at(i).module_json.at("icon");

			status_line += text;

			if (i < state_arr.size() - 1)
				status_line += cfg.separator;
		}

		if (status_line != old_status_line)
		{
			if (verbose)
				std::cout << status_line << '\n';

			if (XStoreName(dpy, DefaultRootWindow(dpy), status_line.c_str()) < 0)
			{
				std::cerr << "failed to set status\n";
				XCloseDisplay(dpy);
				exit(1);
			}
			XFlush(dpy);
			old_status_line = status_line;
		}

		// lets not bully the CPU unnecessarily
		std::this_thread::sleep_for(50ms);
	}
}
