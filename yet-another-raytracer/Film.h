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
    std::vector<uint32_t> sample_counts_;

public:
    explicit Film(uint_vector2 size, tonemap_t tonemap = [](const auto& color) { return color; })
        : size_{std::move(size)}
          , tonemap_(std::move(tonemap))
          , pixels_{std::size_t(size_[0]) * size_[1], color_rgb::zero()}
          , sample_counts_(std::size_t(size_[0]) * size_[1], 0)
    {
    }

    [[nodiscard]] unsigned int width() const { return size_[0]; }
    [[nodiscard]] unsigned int height() const { return size_[1]; }

    void clear() {
        pixels_.assign(size_[0] * size_[1], color_rgb::zero());
        sample_counts_.assign(size_[0] * size_[1], 0);
    }

    void TryLoadFromFile(std::istream& stream);
    void PersistToFile(std::ostream& stream) const;

    [[nodiscard]] std::pair<color_rgb, uint32_t> getPixelData(unsigned int x, unsigned int y) const
    {
        return std::make_pair(pixels_[std::size_t(y) * width() + x], sample_counts_[std::size_t(y) * width() + x]);
    }

    [[nodiscard]] color_rgb getAveragePixelValue(unsigned int x, unsigned int y) const
    {
        const auto index = std::size_t(y) * width() + x;
        return pixels_[index] / sample_counts_[index];
    }

    [[nodiscard]] color_rgb getPixelTonemapped(unsigned int x, unsigned int y) const
    {
        return math::clamp(linear_to_storage(reinhardToneMapping(tonemap_(getAveragePixelValue(x, y)))), color_0, color_1);
    }

    [[nodiscard]] color_u8rgb getPixelTonemappedU8(unsigned int x, unsigned int y) const
    {
        return getPixelTonemapped(x, y) * std::numeric_limits<
            color_u8rgb::element_t>::max();
    }

    void setPixelData(uint_vector2 coord, const std::pair<color_rgb, uint32_t>& data)
    {
        pixels_[std::size_t(coord[1]) * width() + coord[0]] = data.first;
        sample_counts_[std::size_t(coord[1]) * width() + coord[0]] = data.second;
    }

    void registerAtPixel(uint_vector2 coord, const std::pair<color_rgb, uint32_t>& data)
    {
        pixels_[std::size_t(coord[1]) * width() + coord[0]] += data.first;
        sample_counts_[std::size_t(coord[1]) * width() + coord[0]] += data.second;
    }

    // Blits a block from another film.
    // Must support multithreaded access. It is guaranteed that different threads don't write to the same pixels.
    // Copies back aggregated result into sourceFilm
    void transferFilm(Film& sourceFilm, const uint_vector2& destinationMinCoord, const uint_vector2& blockSize)
    {
        for (unsigned int y = 0; y < blockSize[1]; ++y)
        {
            const auto destY = y + destinationMinCoord[1];

            for (unsigned int x = 0; x < blockSize[0]; ++x)
            {
                registerAtPixel({x + destinationMinCoord[0], destY}, sourceFilm.getPixelData(x, y));
                sourceFilm.setPixelData({x, y}, getPixelData(x + destinationMinCoord[0], destY));
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

    static color_rgb lumaBasedReinhardToneMapping(color_rgb color)
    {
        const float luma = dot(color, color_rgb(0.2126, 0.7152, 0.0722));
        const float toneMappedLuma = luma / (1.f + luma);
        color *= toneMappedLuma / luma;
        return color;
    }

    static color_rgb reinhardToneMapping(color_rgb color)
    {
        return color / (color_rgb::one() + color);
    }

    static color_rgb identityToneMapping(color_rgb color)
    {
        return color;
    }
};
