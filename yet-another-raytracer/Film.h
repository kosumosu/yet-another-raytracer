#pragma once

#include "color/color_functions.hpp"
#include "Types.h"

#include <filesystem>
#include <functional>
#include <utility>

class Film
{
public:
    using tonemap_t = std::function<color_rgb(const color_rgb&)>;

private:
    static constexpr color_rgb color_0 = {0.0f, 0.0f, 0.0f};
    static constexpr color_rgb color_1 = {1.0f, 1.0f, 1.0f};

    const uint_vector2 size_;
    const tonemap_t tonemap_;

    std::vector<color_rgb> pixels_;

public:
    explicit Film(uint_vector2 size, tonemap_t tonemap = [](const auto& color) { return color; })
        : size_{std::move(size)}
          , tonemap_(std::move(tonemap))
          , pixels_{std::size_t(size_[0]) * size_[1], color_rgb::zero()}
    {
    }

    [[nodiscard]] unsigned int width() const { return size_[0]; }
    [[nodiscard]] unsigned int height() const { return size_[1]; }

    [[nodiscard]] const std::vector<color_rgb>& pixels() const { return pixels_; }

    [[nodiscard]] const color_rgb& getPixel(unsigned int x, unsigned int y) const
    {
        return pixels_[std::size_t(y) * width() + x];
    }

    [[nodiscard]] color_rgb getPixelTonemapped(unsigned int x, unsigned int y) const
    {
        return math::clamp(linear_to_storage(tonemap_(getPixel(x, y))), color_0, color_1);
    }

    [[nodiscard]] color_u8rgb getPixelTonemappedU8(unsigned int x, unsigned int y) const
    {
        return getPixelTonemapped(x, y) * std::numeric_limits<
            color_u8rgb::element_t>::max();
    }

    void setPixel(uint_vector2 coord, const color_rgb& value)
    {
        pixels_[std::size_t(coord[1]) * width() + coord[0]] = value;
    }

    // Blits a block from another film.
    // Must support multithreaded access. It is guaranteed that different threads don't write to the same pixels.
    void transferFilm(const Film& sourceFilm, const uint_vector2& destinationMinCoord, const uint_vector2& blockSize)
    {
        for (unsigned int y = 0; y < blockSize[1]; ++y)
        {
            const auto destY = y + destinationMinCoord[1];

            for (unsigned int x = 0; x < blockSize[0]; ++x)
            {
                setPixel({x + destinationMinCoord[0], destY}, sourceFilm.getPixel(x, y));
            }
        }
    }

    void SaveAsPng(const std::filesystem::path& filename) const;
    void SaveAsExr(const std::filesystem::path& filename) const;

    [[nodiscard]] uint_vector2 size() const
    {
        return size_;
    }

    [[nodiscard]] Film CreateSubFilm(uint_vector2 size) const
    {
        return Film { std::move(size), tonemap_ };
    }

private:
    enum class TrcMode
    {
        Gamma_22,
        sRGB
    };

    static constexpr TrcMode TRC_MODE = TrcMode::sRGB;

    [[nodiscard]] static color_rgb linear_to_storage(const color_rgb& val)
    {
        if constexpr (TRC_MODE == TrcMode::sRGB)
        {
            return color::linear_to_srgb(val);
        }
        else if (TRC_MODE == TrcMode::Gamma_22)
        {
            return math::pow(val, color_real(1.0f / 2.2f));
        }
        else
        {
            throw;
        }
    }
};
