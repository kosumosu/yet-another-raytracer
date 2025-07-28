#pragma once
#include <iostream>

class debug {
    constexpr static bool COMPILE_ENABLED = false;

    static unsigned int first_disabled_depth;
    static unsigned int current_depth;

public:
    static bool is_enabled() {
        return first_disabled_depth == current_depth;
    }

    static void push_enabled(bool value) {
        if constexpr (COMPILE_ENABLED) {
            if (value) {
                if (is_enabled()) {
                    ++first_disabled_depth;
                }
            } else {
                first_disabled_depth = std::min(first_disabled_depth, current_depth);
            }
            ++current_depth;
        }
    }


    static void pop_enabled() {
        if constexpr (COMPILE_ENABLED) {
            --current_depth;
        }
    }

    template<typename... Args>
    static void print(Args &&... args) {
        if constexpr (COMPILE_ENABLED) {
            if (is_enabled()) {
                (std::cout << ... << args);
            }
        }
    }

    template<typename... Args>
    static void println(Args &&... args) {
        if constexpr (COMPILE_ENABLED) {
            if (is_enabled()) {
                (std::cout << ... << args) << std::endl;
            }
        }
    }
};
