#pragma once

#include "Film.h"

#include <functional>
#include <string>

using area_finished_reporter_t = std::function<void(const Film& result)>;
using area_started_reporter_t = std::function<area_finished_reporter_t(const uint_vector2& top_left, const uint_vector2& bottom_right)>;
using progress_reporter_t = std::function<void(const float progress)>;
using info_printer_t = std::function<void(const std::wstring& text)>;
using rendering_finished_reporter_t = std::function<void(const Film& result)>;
using worker_t = std::function<void(progress_reporter_t report_progress, area_started_reporter_t report_area_started, rendering_finished_reporter_t report_rendering_finished, info_printer_t print_info)>;

namespace applications
{
    class IApplication
    {
    public:
        virtual ~IApplication() = default;
        virtual void run(worker_t worker) = 0;
    };
}
