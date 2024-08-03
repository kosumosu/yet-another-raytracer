#pragma once

#include "IApplication.h"


#include <iomanip>
#include <iostream>
#include <mutex>

namespace applications
{
    class ConsoleApplication final : public IApplication
    {
        std::mutex cout_mutex_;

    public:
        void run(const worker_t worker) override
        {
            worker([this](const float progress)
                   {
                       std::lock_guard guard{cout_mutex_};
                       std::wcout << "Done " << std::setprecision(2) << std::fixed << progress * 100.0f << "%\n";
                   },
                   [this](const std::wstring& text)
                   {
                       std::lock_guard guard{cout_mutex_};
                       std::wcout << text << '\n';
                   }
            );
        }
    };
}
