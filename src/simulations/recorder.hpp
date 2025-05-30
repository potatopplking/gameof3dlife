#pragma once

#include <memory>
#include <iostream>
#include <fstream>

#include "log.hpp"
#include "utilities.hpp"
#include "simulations/base.hpp"

namespace Simulation {

/**
 * Can either directly run and save the simulation,
 * or may act as a passthrough to Window, saving
 * the simulation while it is being run and displayed
 */
class Recorder : public BaseSimulation
{
public:
    Recorder(std::unique_ptr<Simulation::BaseSimulation> sim, std::string filename) :
        m_Simulation(std::move(sim))
    {
        m_File = std::ofstream(filename);
        if (!m_File.is_open()) {
            Log::critical("Failed to open file", filename);
            throw std::runtime_error("Failed to open file");
        }
        Log::info("Recording to file ", filename);
        SaveHeader();
    }

    ~Recorder()
    {
        m_File.close();
    }

    /*
     * Passthrough methods
     * 
     * Used when the recorder is passed as BaseSimulation
     * to the Window (rendering results and recording
     * at the same time)
     */
    void InitRandomState() override
    {
        m_Simulation->InitRandomState();
    }

    double Step(double dt) override {
        // TODO record
        return m_Simulation->Step(dt);
    }

    inline const std::vector<Voxel, utils::TrackingAllocator<Voxel>>& GetVoxels() const override
    {
        return m_Simulation->GetVoxels();
    }

    inline double GetSimulationTime() const override
    {
        return m_Simulation->GetSimulationTime();
    }

    inline double GetStepSize() const override
    {
        return m_Simulation->GetStepSize();
    }

    inline const utils::Vec<int32_t, 3>& GetGridSize() const override
    {
        return m_Simulation->GetGridSize();
    }

    /*
     * Recording related functions
     */

    void Simulate(uint64_t steps) {
        
    }



private:
    std::unique_ptr<Simulation::BaseSimulation> m_Simulation;
    std::ofstream m_File;

    void SaveStep()
    {
        
    }

    void SaveHeader()
    {
        auto [ rows, cols, stacks ] = m_Simulation->GetGridSize().elements;
        m_File.write(reinterpret_cast<char*>(&rows), sizeof(rows));
        m_File.write(reinterpret_cast<char*>(&cols), sizeof(cols));
        m_File.write(reinterpret_cast<char*>(&stacks), sizeof(stacks));
    }
};

}
