#pragma once

#include "IApplication.h"

#include <nana/gui.hpp>
#include <nana/gui/widgets/progress.hpp>

namespace applications
{
    class NanaApplicaion final : IApplication
    {
    public:
        void run(worker_t worker) override
        {
            nana::form form;
            nana::progress progress_bar(form);

            form.events().expose.connect([&]()
            {
                if (!form.visible())
                    return;
                int a = 6456;
            });


            form.show();


            nana::exec();
        }
    };
}
