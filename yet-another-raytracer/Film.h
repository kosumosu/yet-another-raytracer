#pragma once

#include "Types.h"

#include <filesystem>

class Film
{
    static constexpr color_rgb color_0 = {0.0f, 0.0f, 0.0f};
    static constexpr color_rgb color_1 = {1.0f, 1.0f, 1.0f};

    const uint_vector2 size_;
    std::vector<color_rgb> pixels_;

public:
    Film(uint_vector2 size)
        : size_{std::move(size)}
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

    [[nodiscard]] color_u8rgb getPixelTonemapped(unsigned int x, unsigned int y) const
    {
        return math::clamp(linear_to_storage(getPixel(x, y)), color_0, color_1) * std::numeric_limits<color_u8rgb::element_t>::max();
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

    [[nodiscard]] uint_vector2 size() const
    {
        return size_;
    }

private:
    enum class TrcMode
    {
        Gamma_22,
        sRGB
    };

    static constexpr TrcMode TRC_MODE = TrcMode::sRGB;


    [[nodiscard]] static color_real linear_to_storage(color_real val)
    {
        if constexpr (TRC_MODE == TrcMode::sRGB)
        {
            return val < 0.0031308f
                       ? val * 12.92f
                       : 1.055f * powf(val, 1.0f / 2.4f) - 0.055f;
        }
        else if (TRC_MODE == TrcMode::Gamma_22)
        {
            return std::pow(val, color_real(1.0f / 2.2f));
        }
        else
        {
            throw;
        }
    }

    [[nodiscard]] static color_rgb linear_to_storage(const color_rgb& val)
    {
        if constexpr (TRC_MODE == TrcMode::sRGB)
        {
            return color_rgb{
                linear_to_storage(val[0]),
                linear_to_storage(val[1]),
                linear_to_storage(val[2])
            };
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
