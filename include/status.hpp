#pragma once

#include <functional>
#include <map>

#include "config.hpp"

__attribute__((noreturn))
void status_loop(const config& cfg, const bool verbose);
