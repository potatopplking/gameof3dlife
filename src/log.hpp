#pragma once

#include <iostream>

namespace Log {
    enum class LevelTypes {
        CRITICAL = 0, // Things that crash
        ERROR,        // Bad stuff, but we can go on
        WARNING,      // Minor inconvenience
        INFO,         // Normal stuff
        DEBUG,        // Everything. Will slow down execution
    };

    // Set logging level here
    constexpr LevelTypes Level = LevelTypes::DEBUG;

    template <LevelTypes FUNC_LEVEL, typename... Args>
    void _print(Args... args) {
        if constexpr (Level < FUNC_LEVEL) {
            return;
        }
        (std::cout << ... << args) << std::endl;
    }

    template <typename... Args>
    void critical(Args... args) {
        _print<LevelTypes::CRITICAL>(args...);
    }

    template <typename... Args>
    void error(Args... args) {
        _print<LevelTypes::ERROR>(args...);
    }

    template <typename... Args>
    void warning(Args... args) {
        _print<LevelTypes::WARNING>(args...);
    }

    template <typename... Args>
    void info(Args... args) {
        _print<LevelTypes::INFO>(args...);
    }

    template <typename... Args>
    void debug(Args... args) {
        _print<LevelTypes::DEBUG>(args...);
    } 

}
