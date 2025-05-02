#pragma once

#include <vector>

#include "voxel.hpp"

namespace Simulation {

class BaseSimulation {
public:
    virtual ~BaseSimulation() = default;

    // Pure virtual methods to be implemented by derived classes
    virtual void InitRandomState() = 0;
    virtual double Step(double dt) = 0;
    virtual const utils::Vec<int32_t, 3>& GetGridSize() = 0;
    virtual const std::vector<Voxel>& GetVoxels() = 0;

    double GetSimulationTime() const { return simulation_time; }
    double GetStepSize() const { return step; }

protected:
    double simulation_time = 0.0; // Tracks time passed in the simulation
    double step = 1.0;
};

} // namespace Simulation
