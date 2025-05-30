#pragma once

#include <iostream>
#include <fstream>

#include "log.hpp"
#include "simulations/base.hpp"

namespace Simulation {

class Playback : public BaseSimulation
{
public:
    Playback(std::string filename)
    {
        m_File = std::ifstream(filename);
        if (!m_File.is_open()) {
            Log::critical("Failed to open playback file ", filename);
            throw std::runtime_error("Failed to open file");
        }
        Log::info("Opened file ", filename, " for playback");
    }

    ~Playback()
    {

    }

    void InitRandomState()
    {
        throw std::runtime_error("Cannot init random state for playback");
    }

    double Step(double dt)
    {
        // TODO tady jsem skoncil - soucasne implementovat playback a record

        // TODO read step from file and update simulation time
        return 0.0;
    }

private:
    std::ifstream m_File;
};

}
