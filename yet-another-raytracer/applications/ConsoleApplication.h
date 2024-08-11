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
            worker(
                std::stop_token{},
                [](const uint_vector2& film_size) {},
                [this](const size_t nom, const size_t denom)
                   {
                       const float progress = float(nom) / float(denom);
                       std::lock_guard guard{cout_mutex_};
                       std::wcout << "Done " << std::setprecision(2) << std::fixed << progress * 100.0f << "%\n";
                   },
                   [](const auto& top_left, const auto& bottom_right)
                   {
                       return [](const auto& film)
                       {
                       };
                   },
                   [](const auto& film)
                   {
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
