#pragma once

#include "Film.h"

#include <functional>
#include <string>

using initializer_t = std::function<void(const uint_vector2& film_size)>;
using area_finished_reporter_t = std::function<void(const Film& result)>;
using area_started_reporter_t = std::function<area_finished_reporter_t(const uint_vector2& top_left, const uint_vector2& bottom_right)>;
using progress_reporter_t = std::function<void(const size_t nom, const size_t denom)>;
using info_printer_t = std::function<void(const std::wstring& text)>;
using rendering_finished_reporter_t = std::function<void(const Film& result, uint32_t iteration_index)>;
using worker_t = std::function<void(const std::stop_token& stop_token, initializer_t initializer, progress_reporter_t report_progress, area_started_reporter_t report_area_started, rendering_finished_reporter_t report_rendering_finished, info_printer_t print_info)>;

namespace applications
{
    class IApplication
    {
    public:
        virtual ~IApplication() = default;
        virtual void run(worker_t worker) = 0;
    };
}

template <class T>
concept CApplication = std::is_base_of_v<applications::IApplication, T>;