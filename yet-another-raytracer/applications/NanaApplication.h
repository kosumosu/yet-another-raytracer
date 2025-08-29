#pragma once

#if defined(ENABLE_UI_DEF)

#include <iostream>
#include <thread>
#include <nana/gui/widgets/picture.hpp>
#include <nana/paint/pixel_buffer.hpp>

#include "IApplication.h"

#include <nana/gui.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/progress.hpp>

namespace applications
{
    class NanaApplicaion final : IApplication
    {
        const nana::color background_color_ = nana::color(32, 32, 32);
        const nana::color text_color_ = nana::color(188, 188, 188);

        std::mutex mutex_;

    public:
        void run(worker_t worker) override
        {
            nana::form form;
            form.caption("yart");
            form.bgcolor(background_color_);
            nana::place place(form);

            nana::progress progress_bar(form);
            nana::label progress_label(form);
            nana::picture picture(form);
            progress_bar.bgcolor(background_color_);
            progress_label.fgcolor(text_color_);
            //picture.set_gradual_background( background_color_, {16, 16, 16}, false);

            place.div(
                "vert"
                "<progress weight=16"
                "    <progress_bar>"
                "    <weight=4>"
                "    <progress_label weight=60>"
                ">"
                "<picture>"
            );

            place["progress_bar"] << progress_bar;
            place["progress_label"] << progress_label;
            place["picture"] << picture;

            place.collocate();


            //nana::paint::pixel_buffer pb;
            nana::paint::graphics graphics;
            nana::drawing drawing{picture};
            drawing.draw([&graphics, this](nana::paint::graphics& graph)
            {
                graph.bitblt(0, 0, graphics);
            });

            form.show();

            uint32_t iteration = 0;

            std::jthread worker_thread{
                [&, this](const std::stop_token& stop_token)
                {
                    worker(
                        stop_token,
                        [&graphics, &form](const uint_vector2& film_size)
                        {
                            graphics = nana::paint::graphics{{film_size[0], film_size[1]}};
                            form.size({film_size[0], film_size[1] + 16});
                        },
                        [&progress_bar, &progress_label, &form, &iteration, this](const size_t nom, const size_t denom)
                        {
                            std::lock_guard guard{mutex_};
                            progress_bar.amount(denom);
                            progress_bar.value(nom);
                            const auto progress_percent = float(nom) * 100.0f / float(denom);
                            auto progress_text = std::format(L"{:.2f}%", progress_percent);
                            progress_bar.caption(progress_text);
                            progress_label.caption(std::move(progress_text));

                            form.caption(std::format(L"YART {:.2f}% #{}", progress_percent, iteration + 1));
                            //std::wcout << "Done " << std::setprecision(2) << std::fixed << progress * 100.0f << "%\n";
                        },
                        [&drawing, &graphics, /*&pb,*/ this](const auto& top_left, const auto& bottom_right)
                        {
                            std::unique_lock guard{mutex_};
                            draw_bucket_in_progress(graphics, top_left, bottom_right);

                            drawing.update();

                            return [&graphics, /*&pb,*/ &drawing, top_left, bottom_right, this](const Film& film)
                            {
                                const auto size = bottom_right - top_left;

                                std::vector<nana::pixel_color_t> buffer(size[0] * size[1]);

                                for (auto y = 0U; y < size[1]; ++y)
                                {
                                    for (auto x = 0U; x < size[0]; ++x)
                                    {
                                        const auto color = film.getPixelTonemappedU8(x, y);
                                        auto& element = buffer[y * size[0] + x].element;
                                        element.red = color[0];
                                        element.green = color[1];
                                        element.blue = color[2];
                                        element.alpha_channel = 255;
                                    }
                                }

                                nana::paint::pixel_buffer pb{size[0], size[1]};

                                pb.put(reinterpret_cast<const unsigned char*>(
                                           buffer.data()),
                                       size[0],
                                       size[1],
                                       32,
                                       size[0] * sizeof(nana::pixel_color_t),
                                       true);

                                std::unique_lock guard{mutex_};
                                pb.paste(graphics.handle(), {int(top_left[0]), int(top_left[1])});

                                graphics.set_changed();
                                drawing.update();
                            };
                        },
                        [&iteration](const auto& film, size_t iteration_index)
                        {
                            iteration = iteration_index;
                        },
                        [this](const std::wstring& text)
                        {
                            std::unique_lock guard{mutex_};
                            std::wcout << text << '\n';
                        }
                    );
                }
            };

            nana::exec();
        }

    private:
        static nana::rectangle to_nana_rectange(const uint_vector2& top_left,
                                                const uint_vector2& bottom_right)
        {
            const auto size = bottom_right - top_left;
            return {int(top_left[0]), int(top_left[1]), size[0], size[1]};
        }

        static void draw_bucket_in_progress(nana::paint::graphics& graph, const uint_vector2& top_left,
                                            const uint_vector2& bottom_right)
        {
            const nana::color color{nana::colors::tomato};
            const int length = std::min(6, int(math::min_element(bottom_right - top_left)) / 2);
            // not bigger then the bucket itself

            const auto left = int(top_left[0]);
            const auto top = int(top_left[1]);
            const auto right = int(bottom_right[0]) - 1;
            const auto bottom = int(bottom_right[1]) - 1;


            // Top-left corner
            graph.line_begin(left, top + length - 1);
            graph.line_to({left, top}, color);
            graph.line_to({left + length, top}, color);

            // Top-right corner
            graph.line_begin(right - length + 1, top);
            graph.line_to({right, top}, color);
            graph.line_to({right, top + length}, color);

            // Bottom-right corner
            graph.line_begin(right, bottom - length + 1);
            graph.line_to({right, bottom}, color);
            graph.line_to({right - length, bottom}, color);

            // Bottom-left corner
            graph.line_begin(left + length - 1, bottom);
            graph.line_to({left, bottom}, color);
            graph.line_to({left, bottom - length}, color);
        }
    };
}

#else

namespace applications {
    class NanaApplicaion final : IApplication {
    public:
        void run(worker_t worker) override;
    };
}
#endif
