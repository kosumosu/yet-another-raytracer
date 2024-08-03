#pragma once

#include <functional>
#include <string>

using progress_reporter_t = std::function<void(const float progress)>;
using info_printer_t = std::function<void(const std::wstring& text)>;
using worker_t = std::function<void(progress_reporter_t progress_reporter, info_printer_t print_info)>;

namespace applications
{
    class IApplication
    {
    public:
        virtual ~IApplication() = default;
        virtual void run(worker_t worker) = 0;
    };
}
