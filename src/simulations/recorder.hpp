#pragma once

#include <memory>
#include <iostream>
#include <fstream>
#include<typeinfo> // TODO delete

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
        m_File.seekp(0);
        SaveHeader();
    }

    double Step(double dt) override {
        double actual_dt = m_Simulation->Step(dt);
        this->SaveStep(actual_dt);
        return actual_dt;
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

    void SaveStep(double dt)
    {
        auto& voxels = this->GetVoxels();
        m_File.write(reinterpret_cast<char*>(&dt), sizeof(dt));
        auto [ rows, cols, stacks ] = m_Simulation->GetGridSize().elements;
        for (auto row = 0; row < rows; row++) {
            for (auto col = 0; col < cols; col++) {
                for (auto stack = 0; stack < stacks; stack++) {
                    auto& voxel = voxels[IndexFromSimCoords(row, col, stack)];
                    auto [R,G,B,A] = voxel.color.elements;
                    m_File.write(reinterpret_cast<char*>(&R), 1);
                    m_File.write(reinterpret_cast<char*>(&G), 1);
                    m_File.write(reinterpret_cast<char*>(&B), 1);
                    m_File.write(reinterpret_cast<char*>(&A), 1);
                }
            }
        }
    }

    void SaveHeader()
    {
        auto [ rows, cols, stacks ] = m_Simulation->GetGridSize().elements;
        m_File.write(reinterpret_cast<char*>(&rows), sizeof(rows));
        m_File.write(reinterpret_cast<char*>(&cols), sizeof(cols));
        m_File.write(reinterpret_cast<char*>(&stacks), sizeof(stacks));
        this->gridSize.elements[0] = rows;
        this->gridSize.elements[1] = cols;
        this->gridSize.elements[2] = stacks;
    }
};

}
