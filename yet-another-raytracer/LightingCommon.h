#pragma once

#include <functional>

class Flux;

using flux_func = std::function<void(const Flux &)>;